/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <climits>
#include <cfloat>

using namespace std;

#include "eclib/BufferedHandle.h"
#include "eclib/DataHandle.h"
#include "eclib/Exceptions.h"
#include "eclib/FileHandle.h"
#include "eclib/PathName.h"

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
