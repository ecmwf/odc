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

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileHandle.h"
#include "odb_api/odblib/Codec.h"
#include "odb_api/odblib/Column.h"
#include "odb_api/odblib/DataStream.h"
#include "odb_api/odblib/FixedSizeWriterIterator.h"
#include "odb_api/odblib/HashTable.h"
#include "odb_api/odblib/Header.h"
#include "odb_api/odblib/IteratorProxy.h"
#include "odb_api/odblib/MemoryBlock.h"
#include "odb_api/odblib/MetaData.h"
#include "odb_api/odblib/MetaDataReader.h"
#include "odb_api/odblib/MetaDataReaderIterator.h"
#include "odb_api/odblib/RowsIterator.h"
#include "odb_api/odblib/SelectIterator.h"
#include "odb_api/odblib/SQLBitfield.h"
#include "odb_api/odblib/SQLExpression.h"
#include "odb_api/odblib/SQLInteractiveSession.h"
#include "odb_api/odblib/SQLIteratorSession.h"
#include "odb_api/odblib/SQLParser.h"
#include "odb_api/odblib/SQLSelect.h"
#include "odb_api/odblib/SQLTable.h"
#include "odb_api/odblib/SQLType.h"
#include "odb_api/odblib/WriterBufferingIterator.h"
#include "odb_api/odblib/Writer.h"

using namespace std;

namespace odb {

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
    T* it = new T(this->dataHandle(), skipData_);
	it->next();
	return iterator(it);
}

template <typename T>
const typename MetaDataReader<T>::iterator MetaDataReader<T>::end() { return iterator(0); }

} // namespace odb 
