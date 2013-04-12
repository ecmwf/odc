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
/// \file odb_api.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef PUBLIC_ODB_API_H
#define PUBLIC_ODB_API_H

#include <cfloat>
#include <climits>
#include <errno.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdint.h>

#include "eclib/DataHandle.h"
#include "eclib/FileHandle.h"
#include "eclib/PathName.h"
#include "eclib/Resource.h"
#include "eclib/Timer.h"

#include "odblib/ODBAPIVersion.h"
#include "odblib/ODBAPISettings.h"

#include "odblib/Types.h"

#include "odblib/StringTool.h"
#include "odblib/MemoryBlock.h"

#include "odblib/SQLBitfield.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Column.h"
#include "odblib/HashTable.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/MetaData.h"
#include "odblib/RowsIterator.h"
#include "odblib/Header.h"
#include "odblib/IteratorProxy.h"
#include "odblib/TemplateParameters.h"
#include "odblib/Reader.h"
#include "odblib/TextReader.h"
#include "odblib/ODBSelect.h"
#include "odblib/Writer.h"
#include "odblib/WriterBufferingIterator.h"
#include "odblib/WriterDispatchingIterator.h"
#include "odblib/DispatchingWriter.h"
#include "odblib/DispatchingWriter.h"
#include "odblib/ReaderIterator.h"
#include "odblib/TextReaderIterator.h"
#include "odblib/RowsIterator.h"
#include "odblib/SelectIterator.h"
#include "odblib/FixedSizeWriterIterator.h"

#include "odblib/SQLType.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLExpression.h"

#ifndef MEGA
#define MEGA(x) (x*1024*1024)
#endif

using namespace std;

#endif
