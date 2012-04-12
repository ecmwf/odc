///
/// \file ODA.cc
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
#include "Writer.h"
#include "MetaDataReader.h"
#include "MetaDataReaderIterator.h"
#include "WriterBufferingIterator.h"
#include "FixedSizeWriterIterator.h"
#include "RowsIterator.h"
#include "SelectIterator.h"


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

template <typename T>
MetaDataReader<T>::MetaDataReader()
: dataHandle_(0),
  deleteDataHandle_(true),
  path_(""),
  skipData_(true)
{} 

template <typename T>
MetaDataReader<T>::MetaDataReader(const string& path, bool skipData)
: dataHandle_(new FileHandle(path)),
  deleteDataHandle_(true),
  path_(path),
  skipData_(skipData)
{
	dataHandle_->openForRead();
} 

template <typename T>
MetaDataReader<T>::~MetaDataReader()
{
	if (dataHandle_ && deleteDataHandle_)
	{
		dataHandle_->close();
		delete dataHandle_;
	}
}

template <typename T>
typename MetaDataReader<T>::iterator* MetaDataReader<T>::createReadIterator(const PathName& pathName)
{
	return new T(*this, pathName, skipData_);
}

template <typename T>
typename MetaDataReader<T>::iterator MetaDataReader<T>::begin()
{
	T* it = new T(*this, skipData_);
	it->next();
	return iterator(it);
}

template <typename T>
const typename MetaDataReader<T>::iterator MetaDataReader<T>::end() { return iterator(0); }

} // namespace odb 
