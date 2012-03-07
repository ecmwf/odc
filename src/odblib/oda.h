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
/// \file oda.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef PUBLIC_ODA_H
#define PUBLIC_ODA_H

#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <stdint.h>

using namespace std;

#include "LimitValues.h"

#include "ODBAPIVersion.h"
#include "ODBAPISettings.h"

#include "StringTool.h"
#include "PathName.h"
#include "MemoryBlock.h"
#include "DataHandle.h"
#include "FileHandle.h"
#include "Timer.h"
#include "Resource.h"

#include "SQLBitfield.h"
#include "DataStream.h"
#include "HashTable.h"
#include "Codec.h"
#include "Column.h"
#include "HashTable.h"
#include "SQLIteratorSession.h"
#include "MetaData.h"
#include "RowsIterator.h"
#include "Header.h"
#include "IteratorProxy.h"
#include "TemplateParameters.h"
#include "Reader.h"
#include "TextReader.h"
#include "ODBSelect.h"
#include "Writer.h"
#include "WriterBufferingIterator.h"
#include "WriterDispatchingIterator.h"
#include "DispatchingWriter.h"
#include "DispatchingWriter.h"
#include "ReaderIterator.h"
#include "TextReaderIterator.h"
#include "RowsIterator.h"
#include "SelectIterator.h"
#include "FixedSizeWriterIterator.h"

#include "SQLType.h"
#include "SQLInteractiveSession.h"
#include "SQLIteratorSession.h"
#include "SQLTable.h"
#include "SQLSelect.h"
#include "SQLParser.h"
#include "SQLExpression.h"

#ifndef MEGA
#define MEGA(x) (x*1024*1024)
#endif

#endif
