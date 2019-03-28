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

struct TableImpl {
    TableImpl(const core::Table& t) : tables_{t} {}

    /// Add a frame _if_compatible_
    bool addFrame(const core::Table& t);

    const std::vector<ColumnInfo>& columnInfo() const;

    size_t numRows() const;
    size_t numColumns() const;

    void decode(DecodeTarget& target, size_t nthreads);

private: // members

    mutable std::vector<ColumnInfo> columnInfo_;
    std::vector<core::Table> tables_;
};


// Internal API class definition

class OdbImpl {

public: // methods

    OdbImpl(const eckit::PathName& path);
    OdbImpl(eckit::DataHandle& dh);
    OdbImpl(eckit::DataHandle* dh); // takes ownership
    ~OdbImpl();

    Optional<Table> next(bool aggregated, long rowlimit);

private: // members

    core::TablesReader reader_;
    core::TablesReader::iterator it_;
};

//----------------------------------------------------------------------------------------------------------------------

// API Forwarding

Odb::Odb(const std::string& path) :
    impl_(std::make_shared<OdbImpl>(path)) {}

Odb::Odb(eckit::DataHandle& dh) :
    impl_(std::make_shared<OdbImpl>(dh)) {}

Odb::Odb(eckit::DataHandle* dh) :
    impl_(std::make_shared<OdbImpl>(dh)) {}

Odb::~Odb() {}

Optional<Table> Odb::next(bool aggregated, long rowlimit) {
    return impl_->next(aggregated, rowlimit);
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation definition

OdbImpl::OdbImpl(const eckit::PathName& path) :
    reader_(path),
    it_(reader_.begin()) {}

OdbImpl::OdbImpl(eckit::DataHandle& dh) :
    reader_(dh),
    it_(reader_.begin()) {}

OdbImpl::OdbImpl(eckit::DataHandle* dh) :
    reader_(dh),
    it_(reader_.begin()) {}

OdbImpl::~OdbImpl() {}

Optional<Table> OdbImpl::next(bool aggregated, long rowlimit) {

    std::cout << "aggr: " << (aggregated ? "T":"F") << std::endl;

    if (it_ == reader_.end()) return {};

    // !aggregated --> just return the next one
    auto tbl = std::make_shared<TableImpl>(*it_++);
    size_t nrows = tbl->numRows();

    if (aggregated) {
        while (it_ != reader_.end()) {
            if (!tbl->addFrame(*it_)) break;
            ++it_;
        }
    }

    ASSERT(rowlimit < 0 || nrows <= rowlimit);

    return Optional<Table>(tbl);
}

//----------------------------------------------------------------------------------------------------------------------

// Shim for decoding

struct DecodeTargetImpl : public core::DecodeTarget {
    using core::DecodeTarget::DecodeTarget;
};

DecodeTarget::DecodeTarget(const std::vector<std::string>& columns,
                           std::vector<StridedData>& columnFacades) :
    impl_(std::make_shared<DecodeTargetImpl>(columns, columnFacades)) {}

DecodeTarget::~DecodeTarget() {}

//----------------------------------------------------------------------------------------------------------------------

// Table implementation

bool TableImpl::addFrame(const core::Table& t) {

    ASSERT(tables_.size() > 0);
    if (!tables_.front().columns().compatible(t.columns())) return false;
    tables_.push_back(t);
    return true;
}

const std::vector<ColumnInfo>& TableImpl::columnInfo() const {

    ASSERT(tables_.size() > 0);

    // ColumnInfo is memoised, so only constructed once

    if (columnInfo_.empty()) {

        columnInfo_.reserve(numColumns());

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

size_t TableImpl::numRows() const {
    return std::accumulate(tables_.begin(), tables_.end(), size_t(0),
                           [](size_t n, const core::Table& t) { return n + t.numRows(); });
}

size_t TableImpl::numColumns() const {
    return tables_[0].numColumns();
}

void TableImpl::decode(DecodeTarget& target, size_t nthreads) {

    if (tables_.size() == 1) {
        tables_[0].decode(*target.impl_);
    } else {

        std::vector<core::DecodeTarget> targets;

        size_t rowOffset = 0;
        for (core::Table& t : tables_) {
            size_t rows = t.numRows();
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

Table::Table(std::shared_ptr<TableImpl> t) :
    impl_(t) {}

Table::~Table() {}

size_t Table::numRows() const {
    ASSERT(impl_);
    return impl_->numRows();
}

size_t Table::numColumns() const {
    ASSERT(impl_);
    return impl_->numColumns();
}

const std::vector<ColumnInfo>& Table::columnInfo() const {
    ASSERT(impl_);
    return impl_->columnInfo();
}

void Table::decode(DecodeTarget& target, size_t nthreads) const {
    ASSERT(impl_);
    impl_->decode(target, nthreads);
}

//----------------------------------------------------------------------------------------------------------------------

void Settings::treatIntegersAsDoubles(bool flag) {
    odc::ODBAPISettings::instance().treatIntegersAsDoubles(flag);
}

void Settings::setIntegerMissingValue(int64_t val) {
    odc::MDI::integerMDI(val);
}

void Settings::setDoubleMissingValue(double val) {
    odc::MDI::realMDI(val);
}

const std::string& Settings::version() {
    static std::string vstring = LibOdc::instance().version();
    return  vstring;
}

long Settings::integerMissingValue() {
    return odc::MDI::integerMDI();
}

double Settings::doubleMissingValue() {
    return odc::MDI::realMDI();
}

//----------------------------------------------------------------------------------------------------------------------

size_t importText(DataHandle& dh_in, DataHandle& dh_out, const std::string& delimiter) {

    // Convert data handle to std::istream.
    HandleBuf buf(dh_in);
    std::istream is(&buf);

    return importText(is, dh_out, delimiter);
}

size_t importText(std::istream& in, DataHandle& dh_out, const std::string& delimiter) {

    odc::TextReader reader(in, delimiter);
    odc::Writer<> writer(dh_out);
    odc::Writer<>::iterator output(writer.begin());

    return output->pass1(reader.begin(), reader.end());
}

size_t importText(const std::string& in, eckit::DataHandle& dh_out, const std::string& delimiter) {
    MemoryHandle dh_in(in.c_str(), in.length());
    dh_in.openForRead();
    AutoClose close(dh_in);
    return importText(dh_in, dh_out, delimiter);
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
