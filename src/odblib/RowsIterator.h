/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef RowsIterator_H
#define RowsIterator_H

namespace odb {
class Column;
}

//#include "odblib/MetaData.h"

namespace odb {

class RowsReaderIterator 
{
public:
	virtual ~RowsReaderIterator() {}
	virtual MetaData& columns() = 0;

	virtual bool isNewDataset() = 0;
	//virtual double* data() = 0;
protected:
	virtual bool next() = 0;
};

class RowsWriterIterator
{
public:
	virtual ~RowsWriterIterator() {}
	virtual MetaData& columns() = 0;

    virtual int setColumn(size_t index, std::string name, ColumnType type) = 0;
    virtual int setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b) = 0;
	virtual int writeRow(const double* values, unsigned long count) = 0;
};

} // namespace odb

#endif
