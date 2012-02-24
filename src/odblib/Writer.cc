///
/// \file Writer.cc
///
/// @author Piotr Kuchta, Feb 2009


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <values.h>

using namespace std;

#include "Exceptions.h"
#include "PathName.h"
#include "MemoryBlock.h"
#include "DataHandle.h"
#include "FileHandle.h"

#include "SQLBitfield.h"
#include "DataStream.h"
#include "HashTable.h"
#include "Codec.h"
#include "Column.h"
#include "HashTable.h"
#include "SQLBitfield.h"
#include "SQLIteratorSession.h"
#include "MetaData.h"
#include "RowsIterator.h"
#include "Header.h"
#include "IteratorProxy.h"
#include "Reader.h"
#include "Writer.h"
#include "ReaderIterator.h"
#include "WriterBufferingIterator.h"
#include "FixedSizeWriterIterator.h"
#include "RowsIterator.h"
#include "SelectIterator.h"
#include "WriterBufferingIterator.h"


#include "SQLType.h"
#include "SQLInteractiveSession.h"
#include "SQLIteratorSession.h"
#include "SQLTable.h"
#include "SQLSelect.h"
#include "SQLParser.h"
#include "SQLExpression.h"

using namespace std;

namespace odb {
#define MEGA(x) (x*1024*1024)

template <typename ITERATOR>
Writer<ITERATOR>::Writer()
: path_(""),
  dataHandle_(0),
  rowsBufferSize_(Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
  openDataHandle_(true),
  deleteDataHandle_(true)
{} 

template <typename ITERATOR>
Writer<ITERATOR>::Writer(const PathName path)
: path_(path),
  dataHandle_(0),
  rowsBufferSize_(Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
  openDataHandle_(true),
  deleteDataHandle_(true)
{} 

template <typename ITERATOR>
Writer<ITERATOR>::Writer(DataHandle *dh, bool openDataHandle, bool deleteDataHandle)
: path_(""),
  dataHandle_(dh),
  rowsBufferSize_(Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
  openDataHandle_(openDataHandle),
  deleteDataHandle_(deleteDataHandle)
{}

template <typename ITERATOR>
Writer<ITERATOR>::Writer(DataHandle &dh, bool openDataHandle)
: path_(""),
  dataHandle_(&dh),
  rowsBufferSize_(Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
  openDataHandle_(openDataHandle),
  deleteDataHandle_(false)
{}

template <typename ITERATOR>
Writer<ITERATOR>::~Writer() { if (deleteDataHandle_) delete dataHandle_; }

template <typename ITERATOR>
ITERATOR* Writer<ITERATOR>::writer(bool fixedSizeRows)
{
	if (string(path_).size())
	{
		FileHandle *fh = new FileHandle(path_);
		return fixedSizeRows ?
			new FixedSizeWriterIterator(*this, fh)
			: new ITERATOR(*this, fh);
	}
		
	ASSERT(dataHandle_);	

	// TODO: check what iterators do with the data handle (closing, deleting).
	return fixedSizeRows ?
		new FixedSizeWriterIterator(*this, dataHandle_)
		: new ITERATOR(*this, dataHandle_);
}

template <typename ITERATOR>
typename Writer<ITERATOR>::iterator Writer<ITERATOR>::begin(bool openDataHandle)
{
	DataHandle *dh = 0;
	if (string(path_).size())
		dh = new FileHandle(path_);
	else
	{
		ASSERT(dataHandle_);
		dh = dataHandle_;
	}
	return typename Writer::iterator(new ITERATOR(*this, dh, openDataHandle));
}

template <typename ITERATOR>
ITERATOR* Writer<ITERATOR>::createWriteIterator(PathName pathName, bool append)
{
	DataHandle *h = new FileHandle(pathName);
	if (append)
	{
		Length estimatedLength = 10*1024*1024;
		h->openForAppend(estimatedLength);
	}
	return new ITERATOR(*this, h, !append);
}

// Explicit templates' instantiations.

template Writer<WriterBufferingIterator>::Writer();
template Writer<WriterBufferingIterator>::Writer(PathName);
template Writer<WriterBufferingIterator>::Writer(DataHandle&,bool);
template Writer<WriterBufferingIterator>::Writer(DataHandle*,bool,bool);

template Writer<WriterBufferingIterator>::~Writer();
template Writer<WriterBufferingIterator>::iterator Writer<WriterBufferingIterator>::begin(bool);
template WriterBufferingIterator * Writer<WriterBufferingIterator>::createWriteIterator(PathName,bool);

} // namespace odb
