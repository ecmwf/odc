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
