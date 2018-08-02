/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLColumn.h"
#include "eckit/exception/Exceptions.h"

#include "odb_api/TODATableIterator.h"
#include "odb_api/TODATable.h"


namespace odb {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

// TODO: It is not appropriate for parent_.oda() to be const_cast<>-ed.
//       Not entirely clear how to resolve this. But there is no reason for us intrinsically
//       to be modifying the parent. Perhaps we should take a copy of somethnig (oda, dh?)

TODATableIterator::TODATableIterator(const TODATable& parent, const std::vector<std::reference_wrapper<eckit::sql::SQLColumn>>& columns) :
    parent_(parent),
    it_(const_cast<Reader&>(parent_.oda()).begin()),
    end_(parent_.oda().end()),
    columns_(columns),
    firstRow_(true) {

    if (it_ != end_) updateMetaData();
}

void TODATableIterator::rewind() {
    it_ = const_cast<Reader&>(parent_.oda()).begin();
    end_ = parent_.oda().end();
}

TODATableIterator::~TODATableIterator() {}

bool TODATableIterator::next() {

    // We don't need to increment pointer on first row. begin() just called.

    if (firstRow_) {
        firstRow_ = false;
    } else {
		++it_;
    }

    if (it_ == end_) return false;

    if (it_->isNewDataset()) {
        // TODO: Need to update the column pointers in the SQLSelect. AARGH.
        NOTIMP;
        updateMetaData();
    }

	return true;
}


void TODATableIterator::updateMetaData() {

    const MetaData& md = it_->columns();

    for (const eckit::sql::SQLColumn& col : columns_) {

        if (!md.hasColumn(col.name())) {
            throw eckit::UserError("Column \"" + col.name() + "\" not found in table, but required in SQL request", Here());
        }

        size_t idx = md.columnIndex(col.name());
        columnOffsets_.push_back(it_->dataOffset(idx));
    }
}

std::vector<size_t> TODATableIterator::columnOffsets() const {
    ASSERT(columnOffsets_.size() == columns_.size());
    return columnOffsets_;
}

const double* TODATableIterator::data() const {
    return it_->data();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odb
