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

#include <algorithm>
#include <numeric>
#include <mutex>
#include <future>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/HandleBuf.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/BufferList.h"
#include "eckit/log/Log.h"
#include "eckit/utils/StringTools.h"

#include "odc/core/Column.h"
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
#include "odc/Select.h"

using namespace eckit;

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

///
/// Internal types

struct DecoderImpl;

class FrameImpl {

public: // methods

    FrameImpl(std::vector<core::Table>&& tables);

    // Moves this frame onwards
    bool next(bool aggregated, long rowlimit);

    const std::vector<ColumnInfo>& columnInfo() const;
    bool hasColumn(const std::string& column) const;

    size_t rowCount() const;
    size_t columnCount() const;

    eckit::Offset offset() const;
    eckit::Length length() const;

    void decode(DecoderImpl& target, size_t nthreads);
    Span span(const std::vector<std::string>& columns, bool onlyConstantValues);

    Frame filter(const std::string& sql);
    Buffer encodedData();

    const std::map<std::string, std::string>& properties() const;

private: // members

    mutable std::vector<ColumnInfo> columnInfo_;
    std::vector<core::Table> tables_;
    mutable bool propertiesRetrieved_;
    mutable std::map<std::string, std::string> properties_;
};


// Internal API class definition

class ReaderImpl {

public: // methods

    ReaderImpl(const std::string& path, bool aggregated, long rowlimit);
    ReaderImpl(eckit::DataHandle& dh, bool aggregated, long rowlimit);
    ReaderImpl(eckit::DataHandle* dh, bool aggregated, long rowlimit);

    void restart();
    Frame next();

private: // members

    core::TablesReader tablesReader_;
    core::TablesReader::iterator it_;

    long rowlimit_;

    bool aggregated_;
    bool first_;
};

//----------------------------------------------------------------------------------------------------------------------

ReaderImpl::ReaderImpl(const std::string& path, bool aggregated, long rowlimit) :
    tablesReader_(path),
    it_(tablesReader_.begin()),
    rowlimit_(rowlimit),
    aggregated_(aggregated),
    first_(true) {}

ReaderImpl::ReaderImpl(eckit::DataHandle& dh, bool aggregated, long rowlimit) :
    tablesReader_(dh),
    it_(tablesReader_.begin()),
    rowlimit_(rowlimit),
    aggregated_(aggregated),
    first_(true) {}

ReaderImpl::ReaderImpl(eckit::DataHandle* dh, bool aggregated, long rowlimit) :
    tablesReader_(dh),
    it_(tablesReader_.begin()),
    rowlimit_(rowlimit),
    aggregated_(aggregated),
    first_(true) {}

void ReaderImpl::restart() {
    it_ = tablesReader_.begin();
    first_ = true;
}

Frame ReaderImpl::next() {

    std::vector<core::Table> tables;

    if (it_ == tablesReader_.end()) return Frame();

    if (!first_) {
        ++it_;
        if (it_ == tablesReader_.end()) return Frame();
    }

    first_ = false;
    tables.emplace_back(*it_);
    long nrows = tables.back().rowCount();

    if (aggregated_) {
        while (true) {
            auto it_next = it_;
            ++it_next;
            if (it_next == tablesReader_.end()) break;

            long next_nrows = nrows + it_next->rowCount();
            if (rowlimit_ >= 0 && next_nrows > rowlimit_) break;
            if (!tables.front().columns().compatible(it_next->columns())) break;

            ++it_;
            tables.emplace_back(*it_);
            nrows = next_nrows;
        }
    }

    ASSERT(rowlimit_ < 0 || nrows <= rowlimit_);
    return Frame(std::unique_ptr<FrameImpl>(new FrameImpl(std::move(tables))));
}

Buffer FrameImpl::encodedData() {

    eckit::BufferList buffers;
    const bool includeHeader = true;

    for (auto& t : tables_) {
        buffers.append(t.readEncodedData(includeHeader));
    }

    return buffers.consolidate();
}

const std::map<std::string, std::string>& FrameImpl::properties() const {

    ASSERT(!tables_.empty());

    // Properties are memoised, so only filled in once
    if (!propertiesRetrieved_) {

        for (auto& t : tables_) {
            properties_.insert(t.properties().begin(), t.properties().end());
        }

        propertiesRetrieved_ = true;
    }

    return properties_;
}

//----------------------------------------------------------------------------------------------------------------------

// API Forwarding

Reader::Reader(const std::string& path, bool aggregated, long rowlimit) :
    impl_(new ReaderImpl(path, aggregated, rowlimit)) {}

Reader::Reader(eckit::DataHandle& dh, bool aggregated, long rowlimit) :
    impl_(new ReaderImpl(dh, aggregated, rowlimit)) {}

Reader::Reader(eckit::DataHandle* dh, bool aggregated, long rowlimit) :
    impl_(new ReaderImpl(dh, aggregated, rowlimit)) {}

Reader::~Reader() {}

Frame Reader::next() {
    ASSERT(impl_);
    return impl_->next();
}

//----------------------------------------------------------------------------------------------------------------------

// Shim for decoding
struct DecoderImpl : public core::DecodeTarget {
public:
    using core::DecodeTarget::DecodeTarget;
};

Decoder::Decoder(const std::vector<std::string>& columns,
                           std::vector<StridedData>& columnFacades) :
    impl_(new DecoderImpl(columns, columnFacades)) {}

Decoder::~Decoder() {}

void Decoder::decode(const Frame& frame, size_t nthreads) {
    ASSERT(impl_);
    ASSERT(frame.impl_);
    frame.impl_->decode(*impl_, nthreads);
}

Decoder Decoder::slice(size_t rowOffset, size_t nrows) const {
    ASSERT(impl_);
    core::DecodeTarget&& sliced = impl_->slice(rowOffset, nrows);
    return {sliced.columns(), sliced.dataFacades()};
}


//----------------------------------------------------------------------------------------------------------------------

SpanVisitor::~SpanVisitor() {}

//----------------------------------------------------------------------------------------------------------------------

struct SpanImpl : core::Span {
    SpanImpl(core::Span&& s) : core::Span(std::move(s)) {}
    SpanImpl() : core::Span(0, 0) {}
};

Span::Span(std::unique_ptr<SpanImpl>&& s) :
    impl_(std::move(s)) {}

Span::Span() :
    impl_(new SpanImpl()) {}

Span::Span(Span&& s) :
    impl_(std::move(s.impl_)) {}

Span::~Span() {}

bool Span::operator==(const Span& rhs) const {
    return(*impl_ == *rhs.impl_);
}

Span& Span::operator=(Span&& rhs) {
    std::swap(impl_, rhs.impl_);
    return *this;
}

void Span::visit(SpanVisitor& visitor) const {
    impl_->visit(visitor);
}

const std::set<long>& Span::getIntegerValues(const std::string& column) const {
    return impl_->getIntegerValues(column);
}

const std::set<double>& Span::getRealValues(const std::string& column) const {
    return impl_->getRealValues(column);
}

const std::set<std::string>& Span::getStringValues(const std::string& column) const {
    return impl_->getStringValues(column);
}

Offset Span::offset() const {
    return impl_->offset();
}

Length Span::length() const {
    return impl_->length();
}

//----------------------------------------------------------------------------------------------------------------------

// Table implementation

FrameImpl::FrameImpl(std::vector<core::Table>&& tables) :
    tables_(std::move(tables)),
    propertiesRetrieved_(false) {}

const std::vector<ColumnInfo>& FrameImpl::columnInfo() const {

    ASSERT(!tables_.empty());

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

bool FrameImpl::hasColumn(const std::string& column) const {
    ASSERT(!tables_.empty());
    return tables_.front().columns().hasColumn(column);
}

size_t FrameImpl::rowCount() const {
    return std::accumulate(tables_.begin(), tables_.end(), size_t(0),
                           [](size_t n, const core::Table& t) { return n + t.rowCount(); });
}

size_t FrameImpl::columnCount() const {
    ASSERT_MSG(!tables_.empty(), "No tables. Have you remembered to call odc_next_frame() on frame?");
    return tables_[0].columnCount();
}

void FrameImpl::decode(DecoderImpl& target, size_t nthreads) {

    if (tables_.size() == 1) {
        tables_[0].decode(target);
    } else {

        std::vector<core::DecodeTarget> targets;

        size_t rowOffset = 0;
        for (core::Table& t : tables_) {
            size_t rows = t.rowCount();
            core::DecodeTarget&& subTarget(target.slice(rowOffset, rows));
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

namespace {
class SerialTableReadHandle : public DataHandle {
public:
    SerialTableReadHandle(std::vector<core::Table>& tables) :
        tables_(tables),
        buffer_(0) {
        ASSERT(tables.size() > 0);
    }

    Length openForRead() override {
        ASSERT(!tables_.empty());

        table_ = 0;
        pos_ = 0;

        bool includeHeader = true;
        buffer_ = tables_[0].readEncodedData(includeHeader);
        return buffer_.size();
    }

    void close() override {}

    long read(void* out, long length) override {

        if (pos_ == buffer_.size()) {
            if (table_ >= tables_.size()-1) return 0;
            table_++;
            pos_ = 0;
            bool includeHeader = true;
            buffer_ = tables_[table_].readEncodedData(includeHeader);
        }

        ASSERT(pos_ < buffer_.size());
        long readlength = std::min<long>(length, (buffer_.size() - pos_));
        ::memcpy(out, &buffer_[pos_], readlength);
        pos_ += readlength;
        return readlength;
    }

    void print(std::ostream& s) const override {
        s << "SerialTableReadHandle(" << tables_.size() << ")";
    }

    DataHandle *clone() const override {
      return new SerialTableReadHandle(tables_);
    }

private:

    std::vector<core::Table>& tables_;

    Buffer buffer_;
    size_t table_;
    size_t pos_;
};
}

Frame FrameImpl::filter(const std::string& sql) {

    /// @note The SQL functionality works somewhat differently to the rest of the API.
    ///       It parses data in a streaming manner from a data handle.

    // Input data handle

    SerialTableReadHandle input_dh(tables_);

    // Output data handle

    std::unique_ptr<MemoryHandle> output_dh(new MemoryHandle);

    ::odc::api::filter(sql, input_dh, *output_dh);

    // Open this output data handle as a 'new' odb, and extract the Frame
    // There should only be one frame, as it should have a consistent structure from
    // the SQL select

    Reader reader(output_dh.release());
    Frame filtered_frame = reader.next();
    ASSERT(!reader.next());
    return filtered_frame;
}

Span FrameImpl::span(const std::vector<std::string>& columns, bool onlyConstantValues) {

    std::unique_ptr<SpanImpl> s(new SpanImpl(tables_.front().span(columns, onlyConstantValues)));

    for (auto it = tables_.begin() + 1; it != tables_.end(); ++it) {
        s->extend(it->span(columns, onlyConstantValues));
    }

    return Span(std::move(s));
}

eckit::Offset FrameImpl::offset() const {
    return tables_.front().startPosition();
}

eckit::Length FrameImpl::length() const {
    return tables_.back().nextPosition() - tables_.front().startPosition();
}

//----------------------------------------------------------------------------------------------------------------------

Frame::Frame() :
    impl_(nullptr) {}

Frame::Frame(std::unique_ptr<FrameImpl>&& impl) :
    impl_(std::move(impl)) {}

Frame::Frame(const Frame& rhs) :
    impl_(new FrameImpl(*rhs.impl_)) {}

Frame::Frame(Frame&& rhs) :
    impl_(std::move(rhs.impl_)) {}

Frame::~Frame() {}

Frame& Frame::operator=(const Frame& rhs) {
    impl_.reset(new FrameImpl(*rhs.impl_));
    return *this;
}

Frame& Frame::operator=(Frame&& rhs) {
    impl_.reset();
    std::swap(impl_, rhs.impl_);
    return *this;
}

Frame::operator bool() const {
    return !!impl_;
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

bool Frame::hasColumn(const std::string& column) const {
    ASSERT(impl_);
    return impl_->hasColumn(column);
}

Span Frame::span(const std::vector<std::string>& columns, bool onlyConstantValues) const {
    ASSERT(impl_);
    return impl_->span(columns, onlyConstantValues);
}

eckit::Offset Frame::offset() const {
    ASSERT(impl_);
    return impl_->offset();
}

eckit::Length Frame::length() const {
    ASSERT(impl_);
    return impl_->length();
}

Frame Frame::filter(const std::string& sql) {
    ASSERT(impl_);
    return impl_->filter(sql);
}

Buffer Frame::encodedData() {
    ASSERT(impl_);
    return impl_->encodedData();
}

const std::map<std::string, std::string>& Frame::properties() const {
    ASSERT(impl_);
    return impl_->properties();
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

const char* columnTypeName(const ColumnType& type) {

    static const char* names[] = {
        OdbTypes<ColumnType(0)>::name,
        OdbTypes<ColumnType(1)>::name,
        OdbTypes<ColumnType(2)>::name,
        OdbTypes<ColumnType(3)>::name,
        OdbTypes<ColumnType(4)>::name,
        OdbTypes<ColumnType(5)>::name,
    };

    if (int(type) < 0 || size_t(type) >= sizeof(names)/sizeof(names[0])) {
        std::stringstream ss;
        ss << "Unknown type id: " << type;
        throw UserError(ss.str(), Here());
    }

    return names[int(type)];
}

//------------------------------------------------------------------------------------------------------------

size_t odbFromCSV(DataHandle& dh_in, DataHandle& dh_out, const std::string& delimiter) {

    // Convert data handle to std::istream.
    HandleBuf buf(dh_in);
    std::istream is(&buf);
    is.exceptions(std::ios_base::badbit);

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

void encode(DataHandle& out,
            const std::vector<ColumnInfo>& columns,
            const std::vector<ConstStridedData>& data,
            const std::map<std::string, std::string>& properties,
            size_t maxRowsPerFrame) {

    ASSERT(columns.size() == data.size());
    ASSERT(data.size() > 0);

    size_t ncols = data.size();
    size_t nrows = data[0].nelem();
    ASSERT(std::all_of(data.begin(), data.end(), [nrows](const ConstStridedData& d) { return d.nelem() == nrows; }));

    if (nrows <= maxRowsPerFrame) {
        core::encodeFrame(out, columns, data, properties);
    } else {
        std::vector<ConstStridedData> sliced;
        sliced.reserve(ncols);
        size_t start = 0;
        while (start < nrows) {
            size_t nelem = std::min(nrows - start, maxRowsPerFrame);
            for (const ConstStridedData& sd : data) {
                sliced.emplace_back(sd.slice(start, nelem));
            }
            core::encodeFrame(out, columns, sliced, properties);
            start += nelem;
            sliced.clear();
        }
    }
}


size_t filter(const std::string& sql, eckit::DataHandle& in, eckit::DataHandle& out) {

    if (sql.empty()) {
        in.saveInto(out);
    } else {
        odc::Select odb(sql, in);
        odc::Select::iterator it = odb.begin();
        odc::Select::iterator end = odb.end();

        odc::Writer<> writer(out);
        odc::Writer<>::iterator outit = writer.begin();
        return outit->pass1(it, end);
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc
