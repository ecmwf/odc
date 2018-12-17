/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/ReadTablesIterator.h"

#include "eckit/exception/Exceptions.h"

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

ReadTablesIterator::ReadTablesIterator() :
    dh_() {}


ReadTablesIterator::ReadTablesIterator(eckit::DataHandle& dh) :
    dh_(dh) {

    // And get the first table.
    ++(*this);
}


bool ReadTablesIterator::operator!=(const ReadTablesIterator& other) {
    return !(*this == other);
}

bool ReadTablesIterator::operator==(const ReadTablesIterator& other) {
    return dh_ == other.dh_;
}

ReadTablesIterator& ReadTablesIterator::operator++() {

    ASSERT(!dh_.empty());

    dh_.seek(current_.nextPosition());

    if (!Table::readTable(dh_, current_)) {
        dh_ = ThreadSharedDataHandle();
        ASSERT(dh_.empty());
    }

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------

}
}
