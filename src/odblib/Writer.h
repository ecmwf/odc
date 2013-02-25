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
/// \file Writer.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef ODAWRITER_H
#define ODAWRITER_H

#include "odblib/IteratorProxy.h"
#include "odblib/WriterBufferingIterator.h"

namespace eclib { class PathName; }
namespace eclib { class DataHandle; }

namespace odb {

class Reader;
class HashTable;
class SQLIteratorSession;
class RowsReaderIterator;
class ReaderIterator;

typedef WriterBufferingIterator DefaultWritingIterator;

template <typename ITERATOR = DefaultWritingIterator>
class Writer
{
	enum { DEFAULT_ROWS_BUFFER_SIZE = 10000 };
public:
	typedef ITERATOR iterator_class;
	typedef IteratorProxy<ITERATOR, Writer> iterator;

	Writer(const eclib::PathName path);
	Writer(eclib::DataHandle &, bool openDataHandle=true);
	Writer(eclib::DataHandle *, bool openDataHandle=true, bool deleteDataHandle=false);
	Writer();
	virtual ~Writer();

	iterator begin(bool openDataHandle=true);

	eclib::DataHandle& dataHandle() { return *dataHandle_; };

	ITERATOR* createWriteIterator(eclib::PathName, bool append = false);

	unsigned long rowsBufferSize() { return rowsBufferSize_; }
	Writer& rowsBufferSize(unsigned long n) { rowsBufferSize_ = n; }

protected:
	ITERATOR* writer(bool fixedSizeRows = false);

private:
// No copy allowed
    Writer(const Writer&);
    Writer& operator=(const Writer&);

	const eclib::PathName path_;
	eclib::DataHandle* dataHandle_;
	unsigned long rowsBufferSize_;

	bool openDataHandle_;
	bool deleteDataHandle_;
};

/*
template <typename INPUT_ITERATOR, typename OUTPUT_ITERATOR>
Writer<OUTPUT_ITERATOR>& operator<< (Writer<OUTPUT_ITERATOR>&, INPUT_ITERATOR);

template <typename INPUT_ITERATOR, typename OUTPUT_ITERATOR>
Writer<OUTPUT_ITERATOR>& operator<< (Writer<OUTPUT_ITERATOR>& writer, INPUT_ITERATOR in)
{
	typename Writer<OUTPUT_ITERATOR>::iterator outIt = writer.begin(false);

	INPUT_ITERATOR i1 = in;
	outIt->pass1(*i1);
	
	return writer;
}
*/

} // namespace odb

#endif
