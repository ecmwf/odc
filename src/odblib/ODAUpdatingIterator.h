///
/// \file ODAUpdatingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef ODAUpdatingIterator_H
#define ODAUpdatingIterator_H

class PathName;
class DataHandle;

namespace odb {

class Reader;
class Codec;
class HashTable;
class SQLIteratorSession;

template <typename T>
class ODAUpdatingIterator : public RowsReaderIterator
{
public:
	//ODAUpdatingIterator (T& inputIterator, const T& end, const vector<size_t>& columnIndices, const vector<double>& values);
	ODAUpdatingIterator (T& inputIterator, const T& end, const vector<std::string>& columns, const vector<double>& values);
	ODAUpdatingIterator (const T& end);
	~ODAUpdatingIterator ();

	bool isNewDataset();
	double* data() { return data_; }

	virtual MetaData& columns() { return ii_->columns(); }

    ODAUpdatingIterator& operator++() { next(); return *this; }

	bool operator!=(const ODAUpdatingIterator& o) { ASSERT(&o == 0); return ii_ != end_; }

//protected:
	bool next();

private:
// No copy allowed.
    ODAUpdatingIterator(const ODAUpdatingIterator&);
    ODAUpdatingIterator& operator=(const ODAUpdatingIterator&);

	void update();
	void updateIndices();

	// Input iterator.
	T ii_;
	const T& end_;

	vector<std::string> columns_;
	vector<size_t> columnIndices_;
	const vector<double> values_;

	double *data_;
public:
	int refCount_;
	bool noMore_;
};

} // namespace odb 

#include "ODAUpdatingIterator.cc"

#endif
