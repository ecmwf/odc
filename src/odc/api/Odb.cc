/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/api/Odb.h"

#include <numeric>
#include <mutex>
#include <future>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/HandleBuf.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"

#include "odc/core/DecodeTarget.h"
#include "odc/core/Encoder.h"
#include "odc/core/Table.h"
#include "odc/core/TablesReader.h"
#include "odc/csv/TextReader.h"
#include "odc/csv/TextReaderIterator.h"
#include "odc/LibOdc.h"
#include "odc/MDI.h"
#include "odc/ODBAPISettings.h"
#include "odc/Writer.h"

using namespace eckit;

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

///
/// Internal types

struct FrameImpl {
    FrameImpl(Reader& reader);
    FrameImpl(const FrameImpl& rhs);

    // Moves this frame onwards
    bool next(bool aggregated, long rowlimit);

    const std::vector<ColumnInfo>& columnInfo() const;

    size_t rowCount() const;
    size_t columnCount() const;

    void decode(Decoder& target, size_t nthreads);
    Span span(const std::vector<std::string>& columns, bool onlyConstantValues);

private: // members

    mutable std::vector<ColumnInfo> columnInfo_;
    core::TablesReader& reader_;
    core::TablesReader::iterator it_;
    std::vector<core::Table> tables_;
    bool first_;
};


// Internal API class definition

class ReaderImpl : public core::TablesReader {

public: // methods

    using core::TablesReader::TablesReader;
};

//----------------------------------------------------------------------------------------------------------------------

// API Forwarding

Reader::Reader(const std::string& path) :
    impl_(std::make_shared<ReaderImpl>(path)) {}

Reader::Reader(eckit::DataHandle& dh) :
    impl_(std::make_shared<ReaderImpl>(dh)) {}

Reader::Reader(eckit::DataHandle* dh) :
    impl_(std::make_shared<ReaderImpl>(dh)) {}

Reader::~Reader() {}

//----------------------------------------------------------------------------------------------------------------------

// Shim for decoding
struct DecoderImpl : public core::DecodeTarget {
public:
    using core::DecodeTarget::DecodeTarget;
};

Decoder::Decoder(const std::vector<std::string>& columns,
                           std::vector<StridedData>& columnFacades) :
    impl_(std::make_shared<DecoderImpl>(columns, columnFacades)) {}

Decoder::~Decoder() {}

//----------------------------------------------------------------------------------------------------------------------

SpanVisitor::~SpanVisitor() {}

//----------------------------------------------------------------------------------------------------------------------

struct SpanImpl : core::Span {
    SpanImpl(core::Span&& s) : core::Span(std::move(s)) {}
};

Span::Span(std::shared_ptr<SpanImpl> s) : impl_(s) {}

void Span::visit(SpanVisitor& visitor) const {
    impl_->visit(visitor);
}

Offset Span::offset() const {
    return impl_->offset();
}

Length Span::length() const {
    return impl_->length();
}

//----------------------------------------------------------------------------------------------------------------------

// Table implementation

FrameImpl::FrameImpl(Reader& reader) :
    reader_(*reader.impl_),
    it_(reader_.begin()),
    first_(true) {}

FrameImpl::FrameImpl(const FrameImpl& rhs) :
    columnInfo_(rhs.columnInfo_),
    reader_(rhs.reader_),
    it_(rhs.it_),
    tables_(rhs.tables_),
    first_(rhs.first_) {}

bool FrameImpl::next(bool aggregated, long rowlimit) {

    // n.b. Slightly convoluted incrementing of iterator ensures that for a simple read we do it
    // in a single pass, so it will work on a non random-access DataHandle.

    columnInfo_.clear();
    tables_.clear();

    if (it_ == reader_.end()) return false;

    if (!first_) {
        ++it_;
        if (it_ == reader_.end()) return false;
    }

    first_ = false;
    tables_.emplace_back(*it_);
    long nrows = tables_.back().rowCount();

    if (aggregated) {
        while (true) {
            auto it_next = it_;
            ++it_next;
            if (it_next == reader_.end()) break;

            long next_nrows = nrows + it_next->rowCount();
            if (rowlimit >= 0 && next_nrows > rowlimit) break;
            if (!tables_.front().columns().compatible(it_next->columns())) break;

            ++it_;
            tables_.emplace_back(*it_);
            nrows = next_nrows;
        }
    }

    ASSERT(rowlimit < 0 || nrows <= rowlimit);
    return true;
}

const std::vector<ColumnInfo>& FrameImpl::columnInfo() const {

    ASSERT(tables_.size() > 0);

    // ColumnInfo is memoised, so only constructed once

    if (columnInfo_.empty()) {

        columnInfo_.reserve(columnCount());

        for (const core::Column* col : tables_.begin()->columns()) {

            // Extract any bitfield details

            const eckit::sql::BitfieldDef& bf(col->bitfieldDef());

            ASSERT(bf.first.size() == bf.second.size());
            std::vector<ColumnInfo::Bit> bitfield;
            bitfield.reserve(bf.first.size());

            uint8_t offset = 0;
            for (size_t i = 0; i < bf.first.size(); i++) {
                bitfield.emplace_back(ColumnInfo::Bit {
                    bf.first[i],    // name
                    bf.second[i],   // size
                    offset          // offset
                });
                offset += bf.second[i];
            }

            // Construct column details

            columnInfo_.emplace_back(ColumnInfo {
                col->name(),
                col->type(),
                col->dataSizeDoubles() * sizeof(double),
                std::move(bitfield)
            });
        }
    }

    return columnInfo_;
}

size_t FrameImpl::rowCount() const {
    return std::accumulate(tables_.begin(), tables_.end(), size_t(0),
                           [](size_t n, const core::Table& t) { return n + t.rowCount(); });
}

size_t FrameImpl::columnCount() const {
    ASSERT_MSG(!tables_.empty(), "No tables. Have you remembered to call odc_next_frame() on frame?");
    return tables_[0].columnCount();
}

void FrameImpl::decode(Decoder& target, size_t nthreads) {

    if (tables_.size() == 1) {
        tables_[0].decode(*target.impl_);
    } else {

        std::vector<core::DecodeTarget> targets;

        size_t rowOffset = 0;
        for (core::Table& t : tables_) {
            size_t rows = t.rowCount();
            core::DecodeTarget&& subTarget(target.impl_->slice(rowOffset, rows));
            if (nthreads == 1) {
                t.decode(subTarget);
            } else {
                targets.emplace_back(subTarget);
            }
            rowOffset += rows;
        }

        if (nthreads > 1) {
            std::mutex guard_mutex;
            std::vector<std::future<void>> threads;
            size_t next_frame = 0;

            for (size_t i = 0; i < nthreads; i++) {
                threads.emplace_back(std::async(std::launch::async, [&] {
                    while (true) {
                        size_t frame;

                        {
                            std::lock_guard<std::mutex> guard(guard_mutex);
                            if (next_frame < tables_.size()) {
                                frame = next_frame++;
                            } else {
                                return;
                            }
                        }

                        tables_[frame].decode(targets[frame]);
                    }
                }));
            }

            // Waits for the threads. If any exceptions have been thrown, they get thrown into
            // the main thread here.
            for (auto& thread : threads) {
                thread.get();
            }
        }
    }
}

Span FrameImpl::span(const std::vector<std::string>& columns, bool onlyConstantValues) {

    std::shared_ptr<SpanImpl> s(std::make_shared<SpanImpl>(tables_.front().span(columns, onlyConstantValues)));

    for (auto it = tables_.begin() + 1; it != tables_.end(); ++it) {
        s->extend(it->span(columns, onlyConstantValues));
    }

    return s;
}

Frame::Frame(Reader& reader) :
    impl_(new FrameImpl(reader)) {}

Frame::Frame(const Frame& rhs) :
    impl_(new FrameImpl(*rhs.impl_)) {}

Frame::~Frame() {}

bool Frame::next(bool aggregated, long rowlimit) {
    ASSERT(impl_);
    return impl_->next(aggregated, rowlimit);
}

size_t Frame::rowCount() const {
    ASSERT(impl_);
    return impl_->rowCount();
}

size_t Frame::columnCount() const {
    ASSERT(impl_);
    return impl_->columnCount();
}

const std::vector<ColumnInfo>& Frame::columnInfo() const {
    ASSERT(impl_);
    return impl_->columnInfo();
}

void Frame::decode(Decoder& target, size_t nthreads) const {
    ASSERT(impl_);
    impl_->decode(target, nthreads);
}

Span Frame::span(const std::vector<std::string>& columns, bool onlyConstantValues) const {
    ASSERT(impl_);
    return impl_->span(columns, onlyConstantValues);
}

//----------------------------------------------------------------------------------------------------------------------

void Settings::treatIntegersAsDoubles(bool flag) {
    odc::ODBAPISettings::instance().treatIntegersAsDoubles(flag);
}

void Settings::setIntegerMissingValue(long val) {
    odc::MDI::integerMDI(val);
}

void Settings::setDoubleMissingValue(double val) {
    odc::MDI::realMDI(val);
}

const std::string& Settings::version() {
    static std::string vstring = LibOdc::instance().version();
    return  vstring;
}

const std::string& Settings::gitsha1() {
    static std::string vstring = LibOdc::instance().gitsha1(40);
    return  vstring;
}

long Settings::integerMissingValue() {
    return odc::MDI::integerMDI();
}

double Settings::doubleMissingValue() {
    return odc::MDI::realMDI();
}

//----------------------------------------------------------------------------------------------------------------------

size_t odbFromCSV(DataHandle& dh_in, DataHandle& dh_out, const std::string& delimiter) {

    // Convert data handle to std::istream.
    HandleBuf buf(dh_in);
    std::istream is(&buf);

    return odbFromCSV(is, dh_out, delimiter);
}

size_t odbFromCSV(std::istream& in, DataHandle& dh_out, const std::string& delimiter) {

    odc::TextReader reader(in, delimiter);
    odc::Writer<> writer(dh_out);
    odc::Writer<>::iterator output(writer.begin());

    return output->pass1(reader.begin(), reader.end());
}

size_t odbFromCSV(const std::string& in, eckit::DataHandle& dh_out, const std::string& delimiter) {
    MemoryHandle dh_in(in.c_str(), in.length());
    dh_in.openForRead();
    AutoClose close(dh_in);
    return odbFromCSV(dh_in, dh_out, delimiter);
}

void encode(DataHandle& out, const std::vector<ColumnInfo>& columns, const std::vector<ConstStridedData>& data, size_t maxRowsPerFrame) {

    ASSERT(columns.size() == data.size());
    ASSERT(data.size() > 0);

    size_t ncols = data.size();
    size_t nrows = data[0].nelem();
    ASSERT(std::all_of(data.begin(), data.end(), [nrows](const ConstStridedData& d) { return d.nelem() == nrows; }));

    if (nrows <= maxRowsPerFrame) {
        core::encodeFrame(out, columns, data);
    } else {
        std::vector<ConstStridedData> sliced;
        sliced.reserve(ncols);
        size_t start = 0;
        while (start < nrows) {
            size_t nelem = std::min(nrows - start, maxRowsPerFrame);
            for (const ConstStridedData& sd : data) {
                sliced.emplace_back(sd.slice(start, nelem));
            }
            core::encodeFrame(out, columns, sliced);
            start += nelem;
            sliced.clear();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc
