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

#include "odb_api/IteratorProxy.h"
#include "odb_api/TReadOnlyMemoryDataHandle.h"

#include "odb_api/MetaData.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }
namespace ecml { class ExecutionContext; }

extern "C" {
	typedef void oda;
	typedef void oda_read_iterator;
	typedef void oda_write_iterator;
	oda_write_iterator* odb_create_write_iterator(oda*, const char *,int *);
	int odb_read_iterator_get_next_row(oda_read_iterator*, int, double*, int*);
}

namespace odb {
	namespace codec { class Codec; }
	namespace sql { class ODATableIterator; }
	template <typename O> class Header;
}

namespace odb {

class Reader;

class ReaderIterator
{
public:
	ReaderIterator (Reader &owner);
	ReaderIterator (Reader &owner, ecml::ExecutionContext* context);
	ReaderIterator (Reader &owner, const eckit::PathName&);
	ReaderIterator (Reader &owner, const eckit::PathName&, ecml::ExecutionContext*);

	~ReaderIterator ();

	bool isNewDataset();
    const double* data() const { return lastValues_; }
    double* data() { return lastValues_; }

	bool operator!=(const ReaderIterator& other);

	void property(std::string, std::string);
	std::string property(std::string);

    const MetaData& columns() const { return columns_; }
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
    eckit::DataHandle* dataHandle();
//protected:

	int close();

	bool next(ecml::ExecutionContext*);
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

	eckit::DataHandle *f_;
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
    ecml::ExecutionContext* context_;

public:
	int refCount_;
	double& data(size_t);

protected:

	friend ::oda_write_iterator* ::odb_create_write_iterator(::oda*, const char *,int *); // for next()
	friend int ::odb_read_iterator_get_next_row(::oda_read_iterator*, int, double*, int*);

	friend class odb::Reader;
	friend class odb::IteratorProxy<odb::ReaderIterator, odb::Reader, const double>;
	friend class odb::Header<odb::ReaderIterator>;
	friend class odb::sql::ODATableIterator;
};

} // namespace odb

#endif
