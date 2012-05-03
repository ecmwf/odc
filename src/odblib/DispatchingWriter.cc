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
/// \file DispatchingWriter.cc
///
/// @author Piotr Kuchta, June 2009


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <climits>
#include <cfloat>

#include "eclib/Exceptions.h"
#include "eclib/PathName.h"
#include "eclib/DataHandle.h"
#include "eclib/FileHandle.h"

#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/DispatchingWriter.h"
#include "odblib/HashTable.h"
#include "odblib/Header.h"
#include "odblib/IteratorProxy.h"
#include "odblib/MemoryBlock.h"
#include "odblib/MetaData.h"
#include "odblib/Reader.h"
#include "odblib/ReaderIterator.h"
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
#include "odblib/TemplateParameters.h"
#include "odblib/Writer.h"
#include "odblib/WriterDispatchingIterator.h"

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

