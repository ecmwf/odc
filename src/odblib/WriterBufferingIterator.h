/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file WriterBufferingIterator.h
///
/// @author Piotr Kuchta, August 2009

#ifndef WriterBufferingIterator_H
#define WriterBufferingIterator_H

#include <memory>

#include "odblib/Array.h"
#include "odblib/RowsIterator.h"
#include "odblib/UnsafeInMemoryDataHandle.h"
#include "odblib/Writer.h"
#include "odblib/Header.h"

class PathName;
class DataHandle;

namespace odb {

class HashTable;
class SQLIteratorSession;


class WriterBufferingIterator : public RowsWriterIterator
{
public:
	typedef Writer<WriterBufferingIterator> Owner;

	WriterBufferingIterator (Owner &owner, DataHandle *, bool openDataHandle=true);
	~WriterBufferingIterator();

	int open();

	double* data();

	virtual int setColumn(size_t index, std::string name, ColumnType type);
	virtual int setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b);

	void missingValue(size_t i, double); 

	template <typename T> unsigned long pass1(T&, const T&);
	unsigned long gatherStats(const double* values, unsigned long count);

	virtual int close();

	virtual MetaData& columns() { return columns_; }

	Owner& owner() { return owner_; }

	DataHandle& dataHandle() { return *f; }

	void property(string key, string value) { properties_[key] = value; }

//protected:

	template <typename DATASTREAM> int setOptimalCodecs();

	void writeHeader();

	int writeRow(const double* values, unsigned long count);

	size_t rowsBufferSize() { return rowsBufferSize_; }
	void rowsBufferSize(size_t n) { rowsBufferSize_ = n; }

	void flush();
protected:
	bool next();
	Owner& owner_;
	MetaData columns_;
	double* lastValues_;
	double* nextRow_;
	unsigned long nrows_;

	DataHandle *f;
	Array<unsigned char> buffer_;

private:
// No copy allowed.
	WriterBufferingIterator(const WriterBufferingIterator&);
	WriterBufferingIterator& operator=(const WriterBufferingIterator&);

	template <typename T> void pass1init(T&, const T&);

	template <typename T> void doWriteHeader(T&, size_t, size_t);

	void allocBuffers();
	void allocRowsBuffer();
	int doWriteRow(const double*, unsigned long);

	int refCount_;
	Properties properties_;

	Array<unsigned char> blockBuffer_;
	Array<double> rowsBuffer_;
	double* p_;
	FastInMemoryDataHandle memoryDataHandle_;
	MetaData columnsBuffer_;

	size_t rowsBufferSize_;
	MemoryBlock setvBuffer_;
	size_t maxAnticipatedHeaderSize_;

	friend class IteratorProxy<WriterBufferingIterator, Owner>;
	friend class Header<WriterBufferingIterator>;
};

template<typename T>
void WriterBufferingIterator::pass1init(T& it, const T& end)
{
	Log::info() << "WriterBufferingIterator::pass1init" << endl;

	// Copy columns from the input iterator.
	columns() = columnsBuffer_ = it->columns();

	columns().resetStats();
	columnsBuffer_.resetStats();
	
	size_t nCols = it->columns().size();
	ASSERT(nCols > 0);

	allocRowsBuffer();
}

template<typename T>
unsigned long WriterBufferingIterator::pass1(T& it, const T& end)
{
	Log::info() << "WriterBufferingIterator::pass1" << endl;

	pass1init(it, end);

	unsigned long nrows = 0;
	for ( ; it != end; ++it, ++nrows)
	{
		if (it->isNewDataset() && it->columns() != columnsBuffer_)
		{
			Log::info() << "WriterBufferingIterator::pass1: Change of input metadata." << endl;
			flush();
			pass1init(it, end);
			writeHeader();
		}

		const double *data = it->data();
		size_t nCols = it->columns().size();

		gatherStats(data, nCols);

		copy(data, data + nCols, p_);
		p_ += nCols;

		ASSERT(p_ <= rowsBuffer_ + rowsBuffer_.size());
		if (p_ == rowsBuffer_ + rowsBuffer_.size())
			flush();
	} 

	Log::debug() << "Flushing rest of the buffer..." << endl;
	flush();

	Log::info() << "WriterBufferingIterator::pass1: processed " << nrows << " row(s)." << endl;
	ASSERT(close() == 0);
	return nrows;
}

} // namespace odb 

#endif
