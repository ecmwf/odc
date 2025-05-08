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
/// \file ODAUpdatingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef ODAUpdatingIterator_H
#define ODAUpdatingIterator_H

namespace odc {

namespace core {
class MetaData;
}

template <typename T>
class ODAUpdatingIterator {
public:

    ODAUpdatingIterator(T& inputIterator, const T& end, const std::vector<std::string>& columns,
                        const std::vector<double>& values);
    ODAUpdatingIterator(const T& end);
    ~ODAUpdatingIterator();

    bool isNewDataset();
    double* data() { return data_; }

    const core::MetaData& columns() { return ii_->columns(); }
    const core::MetaData& columns(core::MetaData& md) { return ii_->columns(md); }

    ODAUpdatingIterator& operator++() {
        next(0);
        return *this;
    }

    bool operator!=(const ODAUpdatingIterator& o) {
        ASSERT(&o == 0);
        return ii_ != end_;
    }

    // protected:
    bool next();

private:

    // No copy allowed.
    ODAUpdatingIterator(const ODAUpdatingIterator&);
    ODAUpdatingIterator& operator=(const ODAUpdatingIterator&);

    void update();
    void updateIndices();

    // Input iterator.
    T ii_;
    const T& end_;

    std::vector<std::string> columns_;
    std::vector<size_t> columnIndices_;
    const std::vector<double> values_;

    double* data_;

public:

    int refCount_;
    bool noMore_;
};

}  // namespace odc

#include "odc/ODAUpdatingIterator.cc"

#endif
