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
/// \file MetaDataReaderIterator.h
///
/// @author Piotr Kuchta, January 2011

#ifndef MetaDataReaderIterator_H
#define MetaDataReaderIterator_H

#include "SimpleFilterIterator.h"
#include "TReadOnlyMemoryDataHandle.h"


class PathName;
class FileHandle;

extern "C" {
	typedef void oda;
	typedef void oda_read_iterator;
	typedef void oda_write_iterator;
	oda_write_iterator* oda_create_write_iterator(oda*, const char *,int *);
	int oda_read_iterator_get_next_row(oda_read_iterator*, int, double*, int*);
}

namespace odb {
	namespace codec { class Codec; } 
	namespace sql {
		template <typename T> class SQLIteratorSession;
		class ODATableIterator;
	}
}

namespace odb {

class MetaDataReader;

class MetaDataReaderIterator : public RowsReaderIterator
{
public:
	MetaDataReaderIterator (MetaDataReader &owner);
	MetaDataReaderIterator (MetaDataReader &owner, const PathName&);
	~MetaDataReaderIterator ();

	virtual bool isNewDataset();
	const double* data();
	//virtual long integer(int i);

	bool operator!=(const MetaDataReaderIterator& other);

	void property(string, string);
	string property(string);

	virtual MetaData& columns() { return columns_; }

	Offset blockStartOffset() { return blockStartOffset_; }
	Offset blockEndOffset() { return blockEndOffset_; }

	ColumnType columnType(unsigned long index);
	const std::string& columnName(unsigned long index) const;
	const std::string& codecName(unsigned long index) const;
	double columnMissingValue(unsigned long index);
	const BitfieldDef& bitfieldDef(unsigned long index);
//protected:

	virtual int close();

// next() is public because it is used be C API functions
	virtual bool next();
protected:
	bool skip(size_t dataSize);

private:
// No copy allowed.
    MetaDataReaderIterator(const MetaDataReaderIterator&);
    MetaDataReaderIterator& operator=(const MetaDataReaderIterator&);

	void initRowBuffer();
	void loadHeaderAndBufferData();

	MetaDataReader& owner_;
	MetaData columns_;
	double* lastValues_;
	odb::codec::Codec** codecs_;
	unsigned long long nrows_;

	DataHandle *f;
	Properties properties_;

	bool newDataset_;
	bool noMore_;

	bool ownsF_;

	//PrettyFastInMemoryDataHandle
	ReadOnlyMemoryDataHandle memDataHandle_;

	unsigned long headerCounter_;

	Offset blockStartOffset_;
	Offset blockEndOffset_;
protected:
	// FIXME:
    MetaDataReaderIterator(): owner_(*((MetaDataReader *) 0)), columns_(0) {}

	int refCount_;

	//friend ::oda_write_iterator* ::oda_create_write_iterator(::oda*, const char *,int *); // for next()
	//friend int ::oda_read_iterator_get_next_row(::oda_read_iterator*, int, double*, int*);
	friend class odb::MetaDataReader;
	friend class odb::IteratorProxy<odb::MetaDataReaderIterator, odb::MetaDataReader, const double>;
	friend class odb::SimpleFilterIterator<odb::IteratorProxy<odb::MetaDataReaderIterator, odb::MetaDataReader, const double> >;
	friend class odb::Header<odb::MetaDataReaderIterator>;
	friend class odb::sql::ODATableIterator;
};

} // namespace odb

#endif
