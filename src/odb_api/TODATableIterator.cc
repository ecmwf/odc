/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "odb_api/odb_api.h"

#include "odb_api/TODATableIterator.h"

//#include "eckit/sql/SQLDatabase.h"
//#include "eckit/sql/SQLType.h"
//#include "odb_api/TODATable.h"
//#include "odb_api/TODATableIterator.h"



namespace odb {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

TODATableIterator::TODATableIterator(TODATable& parent, const std::vector<std::reference_wrapper<eckit::sql::SQLColumn>>& columns) :
    parent_(p),
    it_(p.oda().begin()),
    end_(p.oda().end()),
    columns_(columns),
    firstRow_(true) {

    if (it_ != end_) updateMetaData();
}

void TODATableIterator::rewind() {
    it_ = parent_.oda().begin();
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

    for (const SQLColumn& col : columns_) {

        if (!md.hasColumn(col.name())) {
            throw UserError("Column \"" + col.name() + "\" not found in table, but required in SQL request", Here());
        }

        size_t idx = md.columnIndex(col.name());
        columnOffsets_.push_back(it_->dataOffset(idx));
    }
}

std::vector<size_t> TODATableIterator::columnOffsets() const {
    ASSERT(columnOffsets_.size() == columns_.size());
    return columnOffsets_;
}

double* TODATableIterator::data() {
    return it_->data();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odb
