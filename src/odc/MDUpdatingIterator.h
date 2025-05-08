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
/// \file MDUpdatingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef MDUpdatingIterator_H
#define MDUpdatingIterator_H

namespace odc {

class MetaData;

template <typename T>
class MDUpdatingIterator {
public:

    MDUpdatingIterator(T& inputIterator, const T& end, const std::vector<std::string>& columns,
                       const std::vector<std::string>& types);
    MDUpdatingIterator(const T& end);
    ~MDUpdatingIterator();

    bool isNewDataset();
    double* data() { return data_; }

    MetaData& columns();  // { return ii_->columns(); }

    MDUpdatingIterator& operator++() {
        next();
        return *this;
    }

    bool operator!=(const MDUpdatingIterator& o) {
        ASSERT(&o == 0);
        return ii_ != end_;
    }

    // protected:
    bool next();

private:

    // No copy allowed.
    MDUpdatingIterator(const MDUpdatingIterator&);
    MDUpdatingIterator& operator=(const MDUpdatingIterator&);

    void update();

    // Input iterator.
    T ii_;
    const T& end_;

    const std::vector<std::string> columns_;
    std::vector<size_t> columnIndices_;
    const std::vector<std::string> types_;
    std::vector<BitfieldDef> bitfieldDefs_;

    MetaData md_;

    double* data_;

public:

    int refCount_;
    bool noMore_;
};

}  // namespace odc

#include "odc/MDUpdatingIterator.cc"

#endif
