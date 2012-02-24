#ifndef RowsIterator_H
#define RowsIterator_H

namespace odb {
class Column;
}

#include "MetaData.h"

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
