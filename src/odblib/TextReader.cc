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
/// @author Piotr Kuchta, Oct 2010


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <climits>
#include <cfloat>

using namespace std;

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
#include "odblib/Reader.h"
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
#include "odblib/TextReader.h"
#include "odblib/TextReaderIterator.h"
#include "odblib/Writer.h"
#include "odblib/WriterBufferingIterator.h"


using namespace std;

namespace odb {
#define MEGA(x) (x*1024*1024)

TextReader::TextReader(std::istream& input)
: in_(&input),
  deleteDataHandle_(false)
{}

TextReader::TextReader()
: in_(0),
  deleteDataHandle_(true),
  path_("")
{}

TextReader::TextReader(const string& path)
: in_(new std::ifstream(path.c_str())),
  deleteDataHandle_(true),
  path_(path)
{
        //dataHandle_->openForRead();
}

TextReader::~TextReader()
{
        ///if (dataHandle_ && deleteDataHandle_)
        if (in_ && deleteDataHandle_)
        {
                //dataHandle_->close();
                //delete dataHandle_;
                delete in_;
        }
}

TextReaderIterator* TextReader::createReadIterator(const PathName& pathName)
{
        return new TextReaderIterator(*this, pathName);
}

TextReader::iterator TextReader::begin()
{
        TextReaderIterator * it = new TextReaderIterator(*this);
        it->next();
        return iterator(it);
}

const TextReader::iterator TextReader::end() { return iterator(0); }

} // namespace odb
