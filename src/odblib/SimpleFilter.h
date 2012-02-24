///
/// \file SimpleFilter.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef SimpleFilter_H
#define SimpleFilter_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "SimpleFilterIterator.h"
#include "IteratorProxy.h"

namespace odb {

template<typename I>
class SimpleFilter
{
public:
	typedef IteratorProxy<SimpleFilterIterator<I>,SimpleFilter,const double> iterator;
	//typedef iterator::Row row;

	SimpleFilter(I b, I e, const string& columnName, const double value);
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

	string columnName_;
	size_t columnIndex_;

	const double value_;

	friend class IteratorProxy<SimpleFilterIterator<I>,SimpleFilter,const double>;
};

} // namespace odb

#include "SimpleFilter.cc"

#endif
