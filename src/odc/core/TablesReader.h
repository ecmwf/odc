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


namespace eckit {
class DataHandle;
}

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

class TablesReader;

class ReadTablesIterator {

public:  // methods

    ReadTablesIterator(TablesReader& owner, long pos = 0);

    // Functionality to work as an iterator

    bool operator!=(const ReadTablesIterator& other);
    bool operator==(const ReadTablesIterator& other);

    ReadTablesIterator& operator++();
    ReadTablesIterator operator++(int);

    Table* operator->();
    const Table* operator->() const;

    Table& operator*();
    const Table& operator*() const;

private:  // methods

    friend std::ostream& operator<<(std::ostream& os, const ReadTablesIterator& rti) {
        os << "ReadTablesIterator(" << &rti.owner_ << ", " << rti.pos_ << ")";
        return os;
    }

private:  // members

    std::reference_wrapper<TablesReader> owner_;
    long pos_;
};

//----------------------------------------------------------------------------------------------------------------------

// TablesReader builds (in a thread safe way) a list of tables that can be queried by
// the iterator. This is done lazily. As a result:
//
// i) Iterators can be instantiated multiple times, but the table structure will only be
//    read once
//
// ii) If data is encoded on the fly, it is all read in one pass. This means that for
//     straightforward workflows, this will work on streaming data that is larger than memory.

class TablesReader {

public:  // types

    using iterator = ReadTablesIterator;

public:  // methods

    TablesReader(eckit::DataHandle& dh);
    TablesReader(eckit::DataHandle* dh);  // n.b. takes ownership
    TablesReader(const eckit::PathName& path);

    iterator begin();
    iterator end();

private:  // members

    friend class ReadTablesIterator;

    bool ensureTable(long idx);
    Table& getTable(long idx);

private:  // members

    std::mutex m_;

    // Vector of pointers --> objects don't move if vector changes after returning reference to object
    std::vector<std::unique_ptr<Table>> tables_;

    ThreadSharedDataHandle dh_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
