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

#include "eclib/Exceptions.h"
#include "eclib/PathName.h"
#include "odblib/MemoryBlock.h"
#include "eclib/DataHandle.h"
#include "eclib/FileHandle.h"

#include "odblib/SQLBitfield.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/HashTable.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/MetaData.h"
#include "odblib/RowsIterator.h"
#include "odblib/Header.h"
#include "odblib/IteratorProxy.h"
#include "odblib/Writer.h"
#include "odblib/SimpleFilter.h"
#include "odblib/ReaderIterator.h"
#include "odblib/WriterBufferingIterator.h"
#include "odblib/FixedSizeWriterIterator.h"
#include "odblib/RowsIterator.h"
#include "odblib/SelectIterator.h"


#include "odblib/SQLType.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLExpression.h"


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
