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
/// \file Select.cc
///
/// @author Piotr Kuchta, April 2010


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
#include "ODBSelect.h"
#include "ReaderIterator.h"
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

Select::Select(const string& selectStatement, DataHandle &dh)
: dataHandle_(&dh),
  deleteDataHandle_(false),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement)
{}

Select::Select(const string& selectStatement, std::istream &is)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(&is),
  deleteIStream_(false),
  selectStatement_(selectStatement)
{}

Select::Select(const string& selectStatement)
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement)
{}

Select::Select()
: dataHandle_(0),
  deleteDataHandle_(true),
  istream_(0),
  deleteIStream_(true),
  selectStatement_()
{}

Select::Select(const string& selectStatement, const string& path)
: dataHandle_(new FileHandle(path)),
  deleteDataHandle_(true),
  // TODO: depending on file extension?
  istream_(0),
  deleteIStream_(true),
  selectStatement_(selectStatement)
{}

Select::~Select()
{
        if (deleteDataHandle_) delete dataHandle_;
        if (deleteIStream_) delete istream_;
}

SelectIterator* Select::createSelectIterator(string sql)
{
        return new SelectIterator(*this, sql);
}

const Select::iterator Select::end() { return iterator(0); }

Select::iterator Select::begin()
{
        SelectIterator* it = new SelectIterator(*this, selectStatement_);
        ASSERT(it);
        it->next();
        return iterator(it);
}

} // namespace odb
