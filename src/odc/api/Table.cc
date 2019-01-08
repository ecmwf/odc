/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/api/Table.h"

#include "odc/api/TableImpl.h"

using namespace eckit;

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

// API Forwarding

Table::Table(std::shared_ptr<TableImpl> t) :
    impl_(t) {}

Table::~Table() {}

size_t Table::numRows() const {
    return impl_->numRows();
}

size_t Table::numColumns() const {
    return impl_->numColumns();
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation definition

TableImpl::TableImpl(const core::Table& t) :
    internal_(t) {}


TableImpl::~TableImpl() {}

size_t TableImpl::numRows() const {
    return internal_.numRows();
}

size_t TableImpl::numColumns() const {
    return internal_.numColumns();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc
