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

extern "C" {
	typedef void oda;
	typedef void oda_read_iterator;
	typedef void oda_write_iterator;
	oda_write_iterator* odb_create_write_iterator(oda*, const char *,int *);
	int odb_read_iterator_get_next_row(oda_read_iterator*, int, double*, int*);
}

namespace odc {
	namespace codec { class Codec; }
	namespace sql { class ODATableIterator; }
	template <typename O> class Header;
}

namespace odc {

class Reader;

class ReaderIterator
{
public:
    ReaderIterator (Reader &owner);
    ReaderIterator (Reader &owner, const eckit::PathName&);

	~ReaderIterator ();

	bool isNewDataset();
    const double* data() const { return lastValues_; }
    double* data() { return lastValues_; }
	double& data(size_t);

	bool operator!=(const ReaderIterator& other);

    void property(std::string, std::string);
    std::string property(std::string);

    const Properties& properties() const { return properties_; }

    const MetaData& columns() const { return columns_; }
    const MetaData& columns(const MetaData& md) { return columns_ = md; }
    void setNumberOfColumns(size_t n) { columns_.setSize(n); }

#ifdef SWIGPYTHON
    int setColumn(size_t, const std::string&, ColumnType) { NOTIMP; }
	void writeHeader() { NOTIMP; }
    int setBitfieldColumn(size_t, const std::string&, ColumnType, eckit::sql::BitfieldDef) { NOTIMP; }
	void missingValue(size_t, double) { NOTIMP; }
#endif

	ColumnType columnType(unsigned long index);
    const std::string& columnName(unsigned long index) const;
    const std::string& codecName(unsigned long index) const;
	double columnMissingValue(unsigned long index);
    const eckit::sql::BitfieldDef& bitfieldDef(unsigned long index);

	int32_t byteOrder() const { return byteOrder_; }
    eckit::DataHandle* dataHandle();
//protected:

	int close();

    bool next();

    /// The offset of a given column in the doubles[] data array
    size_t dataOffset(size_t i) const { ASSERT(columnOffsets_); return columnOffsets_[i]; }

    // Get the number of doubles per row.
    size_t rowDataSizeDoubles() const { return rowDataSizeDoubles_; }

protected:
	size_t readBuffer(size_t dataSize);
    size_t rowDataSizeDoublesInternal() const;

private:
// No copy allowed.
    ReaderIterator(const ReaderIterator&);
    ReaderIterator& operator=(const ReaderIterator&);

	void initRowBuffer();
	void loadHeaderAndBufferData();

    Reader& owner_;
	MetaData columns_;
	double* lastValues_;
    size_t* columnOffsets_; // in doubles
    size_t rowDataSizeDoubles_;
    odc::codec::Codec** codecs_;
	unsigned long long nrows_;

    std::unique_ptr<eckit::DataHandle> f_;
	Properties properties_;

	bool newDataset_;

public:
	bool noMore_;
private:

	//PrettyFastInMemoryDataHandle
	ReadOnlyMemoryDataHandle memDataHandle_;

	unsigned long headerCounter_;
	int32_t byteOrder_;

public:
	int refCount_;

protected:

	friend ::oda_write_iterator* ::odb_create_write_iterator(::oda*, const char *,int *); // for next()
	friend int ::odb_read_iterator_get_next_row(::oda_read_iterator*, int, double*, int*);

	friend class odc::Reader;
	friend class odc::IteratorProxy<odc::ReaderIterator, odc::Reader, const double>;
	friend class odc::Header<odc::ReaderIterator>;
	friend class odc::sql::ODATableIterator;
};

} // namespace odc

#endif
