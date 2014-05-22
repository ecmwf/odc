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
/// \file SimpleFilter.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef SimpleFilter_H
#define SimpleFilter_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "odb_api/SimpleFilterIterator.h"
#include "odb_api/IteratorProxy.h"

namespace odb {

template<typename I>
class SimpleFilter
{
public:
	typedef IteratorProxy<SimpleFilterIterator<I>,SimpleFilter,const double> iterator;
	//typedef iterator::Row row;

	SimpleFilter(I b, I e, const std::string& columnName, const double value);
	SimpleFilter();

	~SimpleFilter();

	iterator begin();
	iterator end(); 

#ifdef SWIGPYTHON
	iterator __iter__() { return begin(); }
#endif

private:
// No copy allowed
    SimpleFilter(const SimpleFilter&);
    SimpleFilter& operator=(const SimpleFilter&);

	I begin_;
	I end_;

	std::string columnName_;
	size_t columnIndex_;

	const double value_;

	friend class IteratorProxy<SimpleFilterIterator<I>,SimpleFilter,const double>;
};

} // namespace odb

#include "odb_api/SimpleFilter.cc"

#endif
