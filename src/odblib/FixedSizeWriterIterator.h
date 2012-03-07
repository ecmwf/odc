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
/// \file FixedSizeWriterIterator.h
///
/// @author Piotr Kuchta, March 2009

#ifndef FixedSizeWriterIterator_H
#define FixedSizeWriterIterator_H

class PathName;
class DataHandle;

namespace odb {

class Codec;
class HashTable;
class SQLIteratorSession;

class FixedSizeWriterIterator : public WriterBufferingIterator 
{
public:
	typedef Writer<WriterBufferingIterator> Owner;

	FixedSizeWriterIterator (Owner& owner, DataHandle *);

	int writeRow(const double* values, unsigned long count);

private:
// No copy allowed.
	FixedSizeWriterIterator(const FixedSizeWriterIterator&);
	FixedSizeWriterIterator& operator=(const FixedSizeWriterIterator&);

};

} // namespace odb 

#endif
