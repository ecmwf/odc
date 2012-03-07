/*
 * © Copyright 1996-2012 ECMWF.
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

using namespace std;

#include "LimitValues.h"

#include "Exceptions.h"
#include "PathName.h"
#include "MemoryBlock.h"
#include "DataHandle.h"
#include "FileHandle.h"
#include "BufferedHandle.h"

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

MetaDataReader::MetaDataReader()
: dataHandle_(0),
  deleteDataHandle_(true),
  path_("")
{}

MetaDataReader::MetaDataReader(const PathName& path)
: dataHandle_(path.fileHandle()),
  deleteDataHandle_(true),
  path_(path)
{
        dataHandle_->openForRead();
}

MetaDataReader::~MetaDataReader()
{
        if (dataHandle_ && deleteDataHandle_)
        {
                dataHandle_->close();
                delete dataHandle_;
        }
}

MetaDataReaderIterator* MetaDataReader::createReadIterator(const PathName& pathName)
{
        return new MetaDataReaderIterator(*this, pathName);
}

MetaDataReader::iterator MetaDataReader::begin()
{
        MetaDataReaderIterator * it = new MetaDataReaderIterator(*this);
        it->next();
        return iterator(it);
}

const MetaDataReader::iterator MetaDataReader::end() { return iterator(0); }

} // namespace odb
