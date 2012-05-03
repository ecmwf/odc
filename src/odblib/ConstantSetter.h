/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ConstantSetter_H
#define ConstantSetter_H

#include "odblib/IteratorProxy.h"
#include "odblib/ODAUpdatingIterator.h"

namespace odb {

template <typename T>
class ConstantSetter
{
public:
	typedef typename odb::ODAUpdatingIterator<T> iterator_class;
	typedef typename odb::IteratorProxy<iterator_class, ConstantSetter, const double> iterator;

	ConstantSetter(const T& b, const T& e, const vector<std::string>& columns, const vector<double>& values)
	: ii_(b), end_(e), columns_(columns), values_(values)
	{}

	~ConstantSetter() {}

	iterator begin() { return iterator(new iterator_class(ii_, end_, columns_, values_)); }
	const iterator end() { return iterator(new iterator_class(end_)); }

private:
	T ii_;
	const T& end_;
	const vector<std::string> columns_;
	const vector<double> values_;
};

} // namespace odb

#include "ConstantSetter.cc"

#endif

