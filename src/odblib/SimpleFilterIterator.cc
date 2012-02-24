///
/// \file SimpleFilterIterator.cc
///
/// @author Piotr Kuchta, June 2009

#include "oda.h"
#include "DataStream.h"
#include "Header.h"
#include "SimpleFilterIterator.h"

namespace odb {

template <typename I>
SimpleFilterIterator<I>::SimpleFilterIterator(I i, I end, size_t columnIndex, double value)
: ii_(i),
  end_(end),
  columnIndex_(columnIndex),
  value_(value),
  refCount_(0),
  noMore_(false)
{
	for (; ii_ != end_ && (*ii_)[columnIndex_] != value_; ++ii_)
		;
	noMore_ = !(ii_ != end_);
}

template <typename I>
SimpleFilterIterator<I>::SimpleFilterIterator(I end)
: ii_(end),
  end_(end),
  columnIndex_(0),
  value_(0),
  refCount_(0),
  noMore_(true)
{}

template <typename I>
SimpleFilterIterator<I>::~SimpleFilterIterator()
{}

template <typename I>
bool SimpleFilterIterator<I>::next()
{
	if (noMore_) return noMore_;

	do 
		++ii_;
	while (ii_ != end_ && (*ii_)[columnIndex_] != value_);

	return noMore_ = !(ii_ != end_);
} // namespace odb

} // namespace odb
