/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MockReader_H
#define MockReader_H

#include "odblib/IteratorProxy.h"

template <typename T>
class MockReader
{
public:
	typedef T iterator_class;
	typedef odb::IteratorProxy<T, MockReader, const double> iterator;

	iterator begin() { return iterator(new T); }
	const iterator end() { return iterator(0); }
};


#endif
