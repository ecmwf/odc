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
/// \file ReaderIterator.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef ReaderIterator_H
#define ReaderIterator_H

#include "odblib/IteratorProxy.h"
//#include "odblib/RowsIterator.h"
#include "odblib/TReadOnlyMemoryDataHandle.h"

#include "odblib/RowsIterator.h"
#include "odblib/MetaData.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

extern "C" {
	typedef void oda;
	typedef void oda_read_iterator;
	typedef void oda_write_iterator;
	oda_write_iterator* odb_create_write_iterator(oda*, const char *,int *);
	int odb_read_iterator_get_next_row(oda_read_iterator*, int, double*, int*);
}

namespace odb {
	namespace codec { class Codec; } 
	namespace sql {
		template <typename T> class SQLIteratorSession;
		class ODATableIterator;
	}
	template <typename I> class SimpleFilterIterator;
	template <typename O> class Header;
}

namespace odb {

class Reader;

class ReaderIterator //: public RowsReaderIterator
{
public:
	ReaderIterator (Reader &owner);
	ReaderIterator (Reader &owner, const eckit::PathName&);
	~ReaderIterator ();

	virtual bool isNewDataset();
	const double* data();

	bool operator!=(const ReaderIterator& other);

	void property(std::string, std::string);
	std::string property(std::string);

	const MetaData& columns() { return columns_; }
	const MetaData& columns(const MetaData& md) { return columns_ = md; }
    void setNumberOfColumns(size_t n) { columns_.setSize(n); }

#ifdef SWIGPYTHON
    int setColumn(size_t, const std::string&, ColumnType) { NOTIMP; }
	void writeHeader() { NOTIMP; }
    int setBitfieldColumn(size_t, const std::string&, ColumnType, BitfieldDef) { NOTIMP; }
	void missingValue(size_t, double) { NOTIMP; }
#endif

	ColumnType columnType(unsigned long index);
    const std::string& columnName(unsigned long index) const;
    const std::string& codecName(unsigned long index) const;
	double columnMissingValue(unsigned long index);
	const BitfieldDef& bitfieldDef(unsigned long index);

	int32_t byteOrder() const { return byteOrder_; }
//protected:

	virtual int close();

	virtual bool next();
protected:
	size_t readBuffer(size_t dataSize);

private:
// No copy allowed.
    ReaderIterator(const ReaderIterator&);
    ReaderIterator& operator=(const ReaderIterator&);

	void initRowBuffer();
	void loadHeaderAndBufferData();

	Reader& owner_;
	MetaData columns_;
	double* lastValues_;
	odb::codec::Codec** codecs_;
	unsigned long long nrows_;

	eckit::DataHandle *f;
	Properties properties_;

	bool newDataset_;

public:
	bool noMore_;
	bool ownsF_;
private:

	//PrettyFastInMemoryDataHandle
	ReadOnlyMemoryDataHandle memDataHandle_;

	unsigned long headerCounter_;
	int32_t byteOrder_;

public:
	int refCount_;
	double& data(size_t);

protected:

	friend ::oda_write_iterator* ::odb_create_write_iterator(::oda*, const char *,int *); // for next()
	friend int ::odb_read_iterator_get_next_row(::oda_read_iterator*, int, double*, int*);

	friend class odb::Reader;
	friend class odb::IteratorProxy<odb::ReaderIterator, odb::Reader, const double>;
	friend class odb::SimpleFilterIterator<odb::IteratorProxy<odb::ReaderIterator, odb::Reader, const double> >;
	friend class odb::Header<odb::ReaderIterator>;
	friend class odb::sql::ODATableIterator;
};

} // namespace odb

#endif
