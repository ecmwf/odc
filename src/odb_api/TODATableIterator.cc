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
#include "odb_api/Reader.h"
#include "odb_api/csv/TextReader.h"
#include "odb_api/csv/TextReaderIterator.h"


namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

// TODO: It is not appropriate for parent_.oda() to be const_cast<>-ed.
//       Not entirely clear how to resolve this. But there is no reason for us intrinsically
//       to be modifying the parent. Perhaps we should take a copy of somethnig (oda, dh?)

template <typename READER>
TODATableIterator<READER>::TODATableIterator(const TODATable<READER>& parent,
                                             const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
                                             std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) :
    parent_(parent),
    it_(const_cast<READER&>(parent_.oda()).begin()),
    end_(parent_.oda().end()),
    columns_(columns),
    metadataUpdateCallback_(metadataUpdateCallback),
    firstRow_(true) {

    if (it_ != end_) updateMetaData();
}

template <typename READER>
void TODATableIterator<READER>::rewind() {
    it_ = const_cast<READER&>(parent_.oda()).begin();
    end_ = parent_.oda().end();
}

template <typename READER>
TODATableIterator<READER>::~TODATableIterator() {}

template <typename READER>
bool TODATableIterator<READER>::next() {

    // We don't need to increment pointer on first row. begin() just called.

    if (firstRow_) {
        firstRow_ = false;
    } else {
		++it_;
    }

    if (it_ == end_) return false;

    if (it_->isNewDataset()) {
        // TODO: Need to update the column pointers in the SQLSelect. AARGH.
        updateMetaData();
        metadataUpdateCallback_(*this);
    }

	return true;
}


template <typename READER>
void TODATableIterator<READER>::updateMetaData() {

    const MetaData& md = it_->columns();

    columnOffsets_.clear();
    for (const eckit::sql::SQLColumn& col : columns_) {

        if (!md.hasColumn(col.name())) {
            throw eckit::UserError("Column \"" + col.name() + "\" not found in table, but required in SQL request", Here());
        }

        size_t idx = md.columnIndex(col.name());
        columnOffsets_.push_back(it_->dataOffset(idx));
    }
}

template <typename READER>
std::vector<size_t> TODATableIterator<READER>::columnOffsets() const {
    ASSERT(columnOffsets_.size() == columns_.size());
    return columnOffsets_;
}

template <typename READER>
const double* TODATableIterator<READER>::data() const {
    return it_->data();
}

// Explicit instantiation

template class TODATableIterator<Reader>;
template class TODATableIterator<TextReader>;

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odc
