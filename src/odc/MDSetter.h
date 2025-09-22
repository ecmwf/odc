/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MDSetter_H
#define MDSetter_H

namespace odc {

template <typename T>
class MDSetter {
public:

    typedef typename odc::MDUpdatingIterator<T> iterator_class;
    typedef typename odc::IteratorProxy<iterator_class, MDSetter, const double> iterator;

    MDSetter(const T& b, const T& e, const std::vector<std::string>& columns, const std::vector<std::string>& types) :
        ii_(b), end_(e), columns_(columns), types_(types) {}

    ~MDSetter() {}

    iterator begin() { return iterator(new iterator_class(ii_, end_, columns_, types_)); }
    const iterator end() { return iterator(new iterator_class(end_)); }

private:

    T ii_;
    const T& end_;
    const std::vector<std::string> columns_;
    const std::vector<std::string> types_;
};

}  // namespace odc

#include "odc/MDSetter.cc"

#endif
