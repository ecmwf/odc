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
/// \file MetaDataReaderIterator.h
///
/// @author Piotr Kuchta, January 2011

#ifndef MetaDataReaderIterator_H
#define MetaDataReaderIterator_H

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "odb_api/Header.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/TReadOnlyMemoryDataHandle.h"

namespace eckit { class PathName; }
namespace eckit { class FileHandle; }
namespace eckit { class DataHandle; }

extern "C" {
	typedef void oda;
	typedef void oda_read_iterator;
	typedef void oda_write_iterator;
	oda_write_iterator* oda_create_write_iterator(oda*, const char *,int *);
	int oda_read_iterator_get_next_row(oda_read_iterator*, int, double*, int*);
}

namespace odc {
	namespace codec { class Codec; } 
	namespace sql { class ODATableIterator; }
}

namespace odc {

class MetaDataReaderIterator 
{
public:

    MetaDataReaderIterator (eckit::DataHandle* handle, bool skipData);
    MetaDataReaderIterator (eckit::DataHandle& handle, bool skipData);
    MetaDataReaderIterator (const eckit::PathName&, bool skipData);
	~MetaDataReaderIterator ();

	bool isNewDataset();
	const double* data();

	bool operator!=(const MetaDataReaderIterator& other);

	void property(std::string, std::string);
	std::string property(std::string);

    const Properties& properties() const { return properties_; }

	const MetaData& columns() { return columns_; }
	const MetaData& columns(MetaData& md) { return columns_ = md; }

	eckit::Offset blockStartOffset() { return blockStartOffset_; }
	eckit::Offset blockEndOffset() { return blockEndOffset_; }

	int32_t byteOrder() const { return byteOrder_; }

	ColumnType columnType(unsigned long index);
	const std::string& columnName(unsigned long index) const;
	const std::string& codecName(unsigned long index) const;
	double columnMissingValue(unsigned long index);
    const eckit::sql::BitfieldDef& bitfieldDef(unsigned long index);
	
	char *encodedData() { return encodedData_; }
	size_t sizeOfEncodedData() { return sizeOfEncodedData_; }
//protected:

    int close();

    bool next();

    eckit::DataHandle* dataHandle();
protected:
    bool skip(size_t dataSize);

private:
// No copy allowed.
    MetaDataReaderIterator(const MetaDataReaderIterator&);
    MetaDataReaderIterator& operator=(const MetaDataReaderIterator&);

    void initRowBuffer();
    void loadHeaderAndBufferData();

    MetaData columns_;
    double* lastValues_;
    odc::codec::Codec** codecs_;
    unsigned long long nrows_;
protected:
    eckit::DataHandle *f_;
    Properties properties_;
    bool newDataset_;
public:
    bool noMore_;

    bool ownsF_;
private:
    ReadOnlyMemoryDataHandle memDataHandle_;

    unsigned long headerCounter_;

    eckit::Offset blockStartOffset_;
    eckit::Offset blockEndOffset_;
    eckit::Length fileSize_; // This will be positive if we read data from a file, not a socket.

protected:
    bool skipData_;
    char *encodedData_;
    size_t sizeOfEncodedData_;
    int32_t byteOrder_;

public:
    int refCount_;

    friend class MetaDataReader<MetaDataReaderIterator>;
    friend class odc::Header<odc::MetaDataReaderIterator>;
    friend class odc::sql::ODATableIterator;
};

} // namespace odc

#endif
