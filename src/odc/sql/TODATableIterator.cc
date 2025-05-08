/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLColumn.h"

#include "odc/Reader.h"
#include "odc/csv/TextReader.h"
#include "odc/csv/TextReaderIterator.h"
#include "odc/sql/TODATable.h"
#include "odc/sql/TODATableIterator.h"


namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

namespace {

/// Return the index of column `columnName` in metadata. If not found or ambiguous,
/// throw an exception.
size_t columnIndex(const std::string& columnName, const core::MetaData& md) {
    size_t idx;
    try {
        idx = md.columnIndex(columnName);
        // Make some error messages more precise
    }
    catch (const core::AmbiguousColumnException& e) {
        throw eckit::UserError("Ambiguous column name \"" + columnName + "\" specified in SQL request.", Here());
    }
    catch (const core::ColumnNotFoundException& e) {
        throw eckit::UserError("Column \"" + columnName + "\" not found in table, but required in SQL request.",
                               Here());
    }
    return idx;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

// TODO: It is not appropriate for parent_.oda() to be const_cast<>-ed.
//       Not entirely clear how to resolve this. But there is no reason for us intrinsically
//       to be modifying the parent. Perhaps we should take a copy of somethnig (oda, dh?)

template <typename READER>
TODATableIterator<READER>::TODATableIterator(
    const TODATable<READER>& parent, const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
    std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback,
    const typename READER::iterator& seedIterator) :
    parent_(parent),
    it_(seedIterator),
    end_(parent_.oda().end()),
    columns_(columns),
    metadataUpdateCallback_(metadataUpdateCallback),
    firstRow_(true) {

    if (it_ != end_)
        updateMetaData();
}

template <typename READER>
void TODATableIterator<READER>::rewind() {
    if (!firstRow_) {
        it_       = const_cast<READER&>(parent_.oda()).begin();
        end_      = parent_.oda().end();
        firstRow_ = true;
    }
}

template <typename READER>
TODATableIterator<READER>::~TODATableIterator() {}

template <typename READER>
bool TODATableIterator<READER>::next() {

    // We don't need to increment pointer on first row. begin() just called.

    if (firstRow_) {
        firstRow_ = false;
    }
    else {
        ++it_;
    }

    if (it_ == end_)
        return false;

    if (it_->isNewDataset()) {
        // TODO: Need to update the column pointers in the SQLSelect. AARGH.
        updateMetaData();
        metadataUpdateCallback_(*this);
    }

    return true;
}


template <typename READER>
void TODATableIterator<READER>::updateMetaData() {

    const core::MetaData& md = it_->columns();

    columnOffsets_.clear();
    columnDoublesSizes_.clear();
    columnsHaveMissing_.clear();
    columnMissingValues_.clear();
    for (const eckit::sql::SQLColumn& col : columns_) {
        const size_t idx = columnIndex(col.name(), md);

        columnOffsets_.push_back(it_->dataOffset(idx));
        columnDoublesSizes_.push_back(it_->dataSizeDoubles(idx));
        columnsHaveMissing_.push_back(it_->hasMissing(idx));
        columnMissingValues_.push_back(it_->missingValue(idx));
    }
}

template <typename READER>
std::vector<size_t> TODATableIterator<READER>::columnOffsets() const {
    ASSERT(columnOffsets_.size() == columns_.size());
    return columnOffsets_;
}

template <typename READER>
std::vector<size_t> TODATableIterator<READER>::doublesDataSizes() const {
    ASSERT(columnDoublesSizes_.size() == columns_.size());
    return columnDoublesSizes_;
}

template <typename READER>
std::vector<char> TODATableIterator<READER>::columnsHaveMissing() const {
    ASSERT(columnsHaveMissing_.size() == columns_.size());
    return columnsHaveMissing_;
}

template <typename READER>
std::vector<double> TODATableIterator<READER>::missingValues() const {
    ASSERT(columnMissingValues_.size() == columns_.size());
    return columnMissingValues_;
}

template <typename READER>
const double* TODATableIterator<READER>::data() const {
    return it_->data();
}

// Explicit instantiation

template class TODATableIterator<Reader>;
template class TODATableIterator<TextReader>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc
