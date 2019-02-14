/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/TablesReader.h"

#include "eckit/exception/Exceptions.h"

using namespace eckit;

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

ReadTablesIterator::ReadTablesIterator(TablesReader& owner, long pos) :
    owner_(owner),
    pos_(pos) {

    // Ensure the first table is loaded
    if (pos_ != -1) {
        if (!owner_.ensureTable(0)) pos_ = -1;
    }
}


bool ReadTablesIterator::operator!=(const ReadTablesIterator& other) {
    return !(*this == other);
}

bool ReadTablesIterator::operator==(const ReadTablesIterator& other) {
    return (&owner_ == &other.owner_) && (pos_ == other.pos_);
}

ReadTablesIterator& ReadTablesIterator::operator++() {

    ++pos_;

    if (!owner_.ensureTable(pos_)) {
        pos_ = -1;
    }

    return *this;
}

ReadTablesIterator ReadTablesIterator::operator++(int) {

    auto copy = *this;
    ++(*this);
    return copy;
}

Table* ReadTablesIterator::operator->() {
    ASSERT(pos_ != -1);
    return &owner_.getTable(pos_);
}

const Table* ReadTablesIterator::operator->() const {
    ASSERT(pos_ != -1);
    return &owner_.getTable(pos_);
}

Table& ReadTablesIterator::operator*() {
    ASSERT(pos_ != -1);
    return owner_.getTable(pos_);
}

const Table& ReadTablesIterator::operator*() const {
    ASSERT(pos_ != -1);
    return owner_.getTable(pos_);
}

//----------------------------------------------------------------------------------------------------------------------


TablesReader::TablesReader(DataHandle& dh) :
    dh_(dh) {}


TablesReader::TablesReader(DataHandle* dh) :
    dh_(dh) {}


TablesReader::TablesReader(const PathName& path) :
    TablesReader(path.fileHandle()) {}


TablesReader::iterator TablesReader::begin() {
    return ReadTablesIterator(*this);
}

TablesReader::iterator TablesReader::end() {
    return ReadTablesIterator(*this, -1);
}

bool TablesReader::ensureTable(long idx) {

    // We only read-ahead by a maximum of one table

    std::lock_guard<std::mutex> lock(m_);

    ASSERT(idx >= 0);
    ASSERT(idx <= long(tables_.size()));

    if (idx == long(tables_.size())) {

        // n.b. Some DataHandles don't implement estimate() --> accept "0"
        Offset nextPosition = (tables_.empty() ? Offset(0) : tables_.back()->nextPosition());
        ASSERT(nextPosition <= dh_.estimate() || dh_.estimate() == Length(0));

        // If the table has been truncated, this is an error, and we cannot read on.
        Offset pos = dh_.seek(nextPosition);
        if (pos < nextPosition) {
            throw ODBIncomplete(dh_.title(), Here());
        }

        std::unique_ptr<Table> tbl(Table::readTable(dh_));
        if (!tbl) return false;
        tables_.emplace_back(std::move(tbl));
    }

    return true;
}

Table& TablesReader::getTable(long idx) {

    ASSERT(idx >= 0);
    ASSERT(idx < long(tables_.size()));

    return *tables_[idx];
}


//----------------------------------------------------------------------------------------------------------------------

}
}
