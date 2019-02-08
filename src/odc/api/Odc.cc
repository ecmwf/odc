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
#include "eckit/log/Log.h"

#include "odc/core/TablesReader.h"
#include "odc/core/Table.h"
#include "odc/core/DecodeTarget.h"

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
    ~OdbImpl();

    bool next(Table& t);

private: // members

    core::TablesReader reader_;
    core::TablesReader::iterator it_;
};

//----------------------------------------------------------------------------------------------------------------------

// API Forwarding

Odb::Odb(const std::string& path) :
    impl_(std::make_shared<OdbImpl>(path)) {}

Odb::~Odb() {}

bool Odb::next(Table& t) {
    return impl_->next(t);
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation definition

OdbImpl::OdbImpl(const eckit::PathName& path) :
    reader_(path),
    it_(reader_.begin()) {}

OdbImpl::~OdbImpl() {}

bool OdbImpl::next(Table& t) {

    if (it_ == reader_.end()) return false;

    t = Table(std::make_shared<TableImpl>(*it_));
    ++it_;

    return true;
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

} // namespace api
} // namespace odc
