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

DecodeTarget::DecodeTarget(std::vector<StridedData>& columnFacades) :
    impl_(std::make_shared<DecodeTargetImpl>(columnFacades)) {}

DecodeTarget::~DecodeTarget() {}

//----------------------------------------------------------------------------------------------------------------------

// Table implementation

Table::Table() {}

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

const std::string& Table::columnName(int col) const {
    ASSERT(impl_);
    ASSERT(col > 0 && size_t(col) < impl_->numColumns());
    return impl_->columns()[col]->name();
}

ColumnType Table::columnType(int col) const {
    ASSERT(impl_);
    ASSERT(col > 0 && size_t(col) < impl_->numColumns());
    return impl_->columns()[col]->type();
}

size_t Table::columnDecodedSize(int col) const {
    ASSERT(impl_);
    return impl_->columns()[col]->dataSizeDoubles() * sizeof(double);
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

void importText(DataHandle& dh_in, DataHandle& dh_out) {

    dh_in.openForRead();
    dh_out.openForWrite(0);

    // Convert data handle to std::istream.
    HandleBuf buf(dh_in);
    std::istream is(&buf);

    odc::TextReader reader(is, ",");
    odc::Writer<> writer(dh_out);
    odc::Writer<>::iterator output(writer.begin());

    output->pass1(reader.begin(), reader.end());
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc
