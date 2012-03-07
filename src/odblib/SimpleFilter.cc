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
#include <values.h>

using namespace std;

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
#include "SimpleFilter.h"
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

template<typename I>
SimpleFilter<I>::SimpleFilter(I b, I e, const string& columnName, const double value)
: begin_(b),
  end_(e),
  columnName_(columnName),
  columnIndex_(0),
  value_(value)
{
	columnIndex_ = b->columns().columnIndex(columnName);
}

template<typename I>
SimpleFilter<I>::~SimpleFilter() {}

template<typename I>
typename SimpleFilter<I>::iterator SimpleFilter<I>::begin()
{
	return iterator (new SimpleFilterIterator<I>(begin_, end_, columnIndex_, value_));
}

template<typename I>
typename SimpleFilter<I>::iterator SimpleFilter<I>::end()
{
	return iterator (new SimpleFilterIterator<I>(end_));
}

} // namespace odb 
