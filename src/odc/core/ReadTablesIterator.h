/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Dec 2018

#ifndef odc_core_ReadTablesIterator_H
#define odc_core_ReadTablesIterator_H

#include <cstdint>
#include <memory>

#include "odc/core/Table.h"
#include "odc/core/ThreadSharedDataHandle.h"


namespace eckit { class DataHandle; }

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

class ReadTablesIterator {

public: // methods

    ReadTablesIterator();
    ReadTablesIterator(eckit::DataHandle& dh);

    // Functionality to work as an iterator

    bool operator!=(const ReadTablesIterator& other);
    bool operator==(const ReadTablesIterator& other);

    // n.b. We only define the prefix increment operator
    ReadTablesIterator& operator++();

    Table* operator->() { return &current_; }
    const Table* operator->() const { return &current_; }

    Table& operator*() { return current_; }
    const Table& operator*() const { return current_; }

private: // members

    ThreadSharedDataHandle dh_;
    Table current_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

#endif
