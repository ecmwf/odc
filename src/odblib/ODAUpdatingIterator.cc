///
/// \file ODAUpdatingIterator.cc
///
/// @author Piotr Kuchta, June 2009

#include "oda.h"
#include "DataStream.h"
#include "Header.h"
#include "ODAUpdatingIterator.h"

namespace odb {

template <typename T>
ODAUpdatingIterator<T>::ODAUpdatingIterator (T& ii, const T& end, const vector<std::string>& columns, const vector<double>& values)
: ii_(ii),
  end_(end),
  columns_(columns),
  columnIndices_(columns.size()), 
  values_(values),
  data_(0),
  refCount_(0),
  noMore_(false)
{
	ASSERT(columns.size() == values.size());

	updateIndices();
	copy(ii_->data(), ii_->data() + ii_->columns().size(), data_);
	update();
}

template <typename T>
void ODAUpdatingIterator<T>::updateIndices()
{
	MetaData& md = ii_->columns();

	delete [] data_;
	data_ = new double[md.size()];

	for (size_t i = 0; i < columns_.size(); ++i)
		columnIndices_[i] = md.columnIndex(columns_[i]);
}

template <typename T>
ODAUpdatingIterator<T>::ODAUpdatingIterator (const T& end)
: ii_(end),
  end_(end),
  columnIndices_(),
  values_(),
  data_(0),
  refCount_(0),
  noMore_(true)
{}

template <typename T>
ODAUpdatingIterator<T>::~ODAUpdatingIterator () { delete [] data_; }

template <typename T>
void ODAUpdatingIterator<T>::update()
{
	for (size_t i = 0; i < columnIndices_.size(); ++i)
		data_[columnIndices_[i]] = values_[i];
}

template <typename T>
bool ODAUpdatingIterator<T>::isNewDataset()
{
	return ii_->isNewDataset();
}

template <typename T>
bool ODAUpdatingIterator<T>::next()
{
	if (noMore_)
		return noMore_;
	++ii_;
	bool r = ii_ != end_;
	if (r)
	{
		if (ii_->isNewDataset())
			updateIndices();

		copy(ii_->data(), ii_->data() + ii_->columns().size(), data_);
		update();
	}
	noMore_ = !r;
	return r;
}

} // namespace odb

