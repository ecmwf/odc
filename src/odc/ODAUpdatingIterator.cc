/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file ODAUpdatingIterator.cc
///
/// @author Piotr Kuchta, June 2009

#include "odc/core/MetaData.h"

namespace odc {

template <typename T>
ODAUpdatingIterator<T>::ODAUpdatingIterator(T& ii, const T& end, const std::vector<std::string>& columns,
                                            const std::vector<double>& values) :
    ii_(ii),
    end_(end),
    columns_(columns),
    columnIndices_(columns.size()),
    values_(values),
    data_(0),
    refCount_(0),
    noMore_(false) {
    ASSERT(columns.size() == values.size());

    updateIndices();
    std::copy(ii_->data(), ii_->data() + ii_->columns().size(), data_);
    update();
}

template <typename T>
void ODAUpdatingIterator<T>::updateIndices() {
    const core::MetaData& md(ii_->columns());

    delete[] data_;
    data_ = new double[md.size()];

    for (size_t i = 0; i < columns_.size(); ++i)
        columnIndices_[i] = md.columnIndex(columns_[i]);
}

template <typename T>
ODAUpdatingIterator<T>::ODAUpdatingIterator(const T& end) :
    ii_(end), end_(end), columnIndices_(), values_(), data_(0), refCount_(0), noMore_(true) {}

template <typename T>
ODAUpdatingIterator<T>::~ODAUpdatingIterator() {
    delete[] data_;
}

template <typename T>
void ODAUpdatingIterator<T>::update() {
    for (size_t i = 0; i < columnIndices_.size(); ++i)
        data_[columnIndices_[i]] = values_[i];
}

template <typename T>
bool ODAUpdatingIterator<T>::isNewDataset() {
    return ii_->isNewDataset();
}

template <typename T>
bool ODAUpdatingIterator<T>::next() {
    if (noMore_)
        return noMore_;
    ++ii_;
    bool r = ii_ != end_;
    if (r) {
        if (ii_->isNewDataset())
            updateIndices();

        std::copy(ii_->data(), ii_->data() + ii_->columns().size(), data_);
        update();
    }
    noMore_ = !r;
    return r;
}

}  // namespace odc
