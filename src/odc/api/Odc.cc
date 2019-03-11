/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/api/Odc.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/io/HandleBuf.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"

#include "odc/core/TablesReader.h"
#include "odc/core/Table.h"
#include "odc/core/DecodeTarget.h"
#include "odc/csv/TextReader.h"
#include "odc/csv/TextReaderIterator.h"
#include "odc/Writer.h"
#include "odc/MDI.h"
#include "odc/ODBAPISettings.h"

using namespace eckit;

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

///
/// Internal types

struct TableImpl : public core::Table {
    TableImpl(const core::Table& t) : Table(t) {}

    const std::vector<ColumnInfo>& columnInfo() const;

private: // members

    mutable std::vector<ColumnInfo> columnInfo_;
};


// Internal API class definition

class OdbImpl {

public: // methods

    OdbImpl(const eckit::PathName& path);
    OdbImpl(eckit::DataHandle& dh);
    OdbImpl(eckit::DataHandle* dh); // takes ownership
    ~OdbImpl();

    Optional<Table> next();

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

Optional<Table> Odb::next() {
    return impl_->next();
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

Optional<Table> OdbImpl::next() {

    if (it_ == reader_.end()) return {};

    return Optional<Table>(std::make_shared<TableImpl>(*it_++));
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

const std::vector<ColumnInfo>& TableImpl::columnInfo() const {

    // ColumnInfo is memoised, so only constructed once

    if (columnInfo_.empty()) {

        columnInfo_.reserve(numColumns());

        for (const core::Column* col : columns()) {

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

void Table::decode(DecodeTarget& target) const {
    ASSERT(impl_);
    impl_->decode(*target.impl_);
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

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc
