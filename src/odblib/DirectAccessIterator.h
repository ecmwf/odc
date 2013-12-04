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
/// \file DirectAccessIterator.h
///
/// @author Baudouin Raoult, Dec 2013


#ifndef DirectAccessIterator_H
#define DirectAccessIterator_H

#include "odblib/RowsIterator.h"
#include "odblib/MetaDataReaderIterator.h"
#include "odblib/MetaDataReader.h"

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

class DirectAccess;

class DirectAccessIterator : public RowsReaderIterator, private MetaDataReader<MetaDataReaderIterator>
{
public:
    DirectAccessIterator (DirectAccess &owner);
    DirectAccessIterator (DirectAccess &owner, const eckit::PathName&);
	~DirectAccessIterator ();

	virtual bool isNewDataset();
	const double* data();

	bool operator!=(const DirectAccessIterator& other);

	void property(std::string, std::string);
	std::string property(std::string);

	virtual MetaData& columns() { return columns_; }

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

private:
// No copy allowed.
    DirectAccessIterator(const DirectAccessIterator&);
    DirectAccessIterator& operator=(const DirectAccessIterator&);

    void initBlocks();
	void initRowBuffer();

    DirectAccess& owner_;
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

    std::deque<std::pair<eckit::Offset, eckit::Length> > blocks_;

	unsigned long headerCounter_;
	int32_t byteOrder_;

    eckit::Offset lastOffset_;

public:
	int refCount_;
	double& data(size_t);

protected:

	friend ::oda_write_iterator* ::odb_create_write_iterator(::oda*, const char *,int *); // for next()
	friend int ::odb_read_iterator_get_next_row(::oda_read_iterator*, int, double*, int*);

	friend class odb::Reader;
	friend class odb::IteratorProxy<odb::DirectAccessIterator, odb::Reader, const double>;
	friend class odb::SimpleFilterIterator<odb::IteratorProxy<odb::DirectAccessIterator, odb::Reader, const double> >;
	friend class odb::Header<odb::DirectAccessIterator>;
	friend class odb::sql::ODATableIterator;
};

} // namespace odb

#endif
