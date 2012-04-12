///
/// \file MDUpdatingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef MDUpdatingIterator_H
#define MDUpdatingIterator_H

class PathName;
class DataHandle;

namespace odb {

class Reader;
class Codec;
class HashTable;
class SQLIteratorSession;
class MetaData;

template <typename T>
class MDUpdatingIterator : public RowsReaderIterator
{
public:
	MDUpdatingIterator (T& inputIterator, const T& end, const vector<string>& columns, const vector<string>& types);
	MDUpdatingIterator (const T& end);
	~MDUpdatingIterator ();

	bool isNewDataset();
	double* data() { return data_; }

	virtual MetaData& columns(); // { return ii_->columns(); }

    MDUpdatingIterator& operator++() { next(); return *this; }

	bool operator!=(const MDUpdatingIterator& o) { ASSERT(&o == 0); return ii_ != end_; }

//protected:
	bool next();

private:
// No copy allowed.
    MDUpdatingIterator(const MDUpdatingIterator&);
    MDUpdatingIterator& operator=(const MDUpdatingIterator&);

	void update();

	// Input iterator.
	T ii_;
	const T& end_;

	const vector<std::string> columns_;
	vector<size_t> columnIndices_;
	const vector<string> types_;
	vector<BitfieldDef> bitfieldDefs_;

	MetaData md_;

	double *data_;
public:
	int refCount_;
	bool noMore_;
};

} // namespace odb 

#include "MDUpdatingIterator.cc"

#endif
