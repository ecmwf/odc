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
/// \file SimpleFilterIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef SimpleFilterIterator_H
#define SimpleFilterIterator_H

namespace eclib { class PathName; }
namespace eclib { class DataHandle; }

namespace odb {

template <typename I>
class SimpleFilterIterator 
{
public:
	SimpleFilterIterator (I i, I end, size_t columnIndex, const double value);
	SimpleFilterIterator (I end);
	~SimpleFilterIterator ();

	bool isNewDataset() { return ii_.isNewDataset(); }
	const double* data() { return ii_->data(); }

	const MetaData& columns() { return ii_->columns(); }

//protected:
	bool next();
private:
// No copy allowed.
    SimpleFilterIterator(const SimpleFilterIterator&);
    SimpleFilterIterator& operator=(const SimpleFilterIterator&);

	unsigned long long nrows_;

	// Input iterator.
	I ii_;
	I end_;
	size_t columnIndex_;
	const double value_;
public:
 ///FIXME: private
	int refCount_;
	bool noMore_;
};

} // namespace odb

#include "SimpleFilterIterator.cc"

#endif
