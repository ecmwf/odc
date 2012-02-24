///
/// \file SimpleFilterIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef SimpleFilterIterator_H
#define SimpleFilterIterator_H

class PathName;
class DataHandle;

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

	virtual MetaData& columns() { return ii_->columns(); }


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
