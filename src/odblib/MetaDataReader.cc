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
/// \file ODA.cc
///
/// @author Piotr Kuchta, Feb 2009


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
//#include <values.h>

using namespace std;

#include "eclib/DataHandle.h"
#include "eclib/Exceptions.h"
#include "eclib/filesystem/FileHandle.h"
#include "eclib/filesystem/PathName.h"

#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/FixedSizeWriterIterator.h"
#include "odblib/HashTable.h"
#include "odblib/Header.h"
#include "odblib/IteratorProxy.h"
#include "odblib/MemoryBlock.h"
#include "odblib/MetaData.h"
#include "odblib/MetaDataReader.h"
#include "odblib/MetaDataReaderIterator.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLType.h"
#include "odblib/SelectIterator.h"
#include "odblib/Writer.h"
#include "odblib/WriterBufferingIterator.h"

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
MetaDataReader<T>::MetaDataReader(const eckit::PathName& path, bool skipData)
: dataHandle_(path.fileHandle()),
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
typename MetaDataReader<T>::iterator* MetaDataReader<T>::createReadIterator(const eckit::PathName& pathName)
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
