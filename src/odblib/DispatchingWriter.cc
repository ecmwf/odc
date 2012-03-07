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
/// \file DispatchingWriter.cc
///
/// @author Piotr Kuchta, June 2009


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>

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
#include "Reader.h"
#include "Writer.h"
#include "TemplateParameters.h"
#include "WriterDispatchingIterator.h"
#include "DispatchingWriter.h"
#include "ReaderIterator.h"
#include "SelectIterator.h"


#include "SQLType.h"
#include "SQLInteractiveSession.h"
#include "SQLIteratorSession.h"
#include "SQLTable.h"
#include "SQLSelect.h"
#include "SQLParser.h"
#include "SQLExpression.h"

namespace odb {
#define MEGA(x) (x*1024*1024)

DispatchingWriter::DispatchingWriter(const string& outputFileTemplate, int maxOpenFiles)
: outputFileTemplate_(outputFileTemplate),
  maxOpenFiles_(maxOpenFiles ? maxOpenFiles :  Resource<long>("$ODBAPI_MAX_OPEN_FILES;-maxOpenFiles;maxOpenFiles", 250))
{}

DispatchingWriter::~DispatchingWriter() {}

DispatchingWriter::iterator_class* DispatchingWriter::writer()
{
        NOTIMP;
        // TODO:
        return 0;
}

DispatchingWriter::iterator DispatchingWriter::begin()
{
	return iterator(new iterator_class(*this, maxOpenFiles_));
}

} // namespace odb

