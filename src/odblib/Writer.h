///
/// \file Writer.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef ODAWRITER_H
#define ODAWRITER_H

class PathName;
class DataHandle;

namespace odb {

class Reader;
class HashTable;
class SQLIteratorSession;
class RowsReaderIterator;
class ReaderIterator;
class WriterBufferingIterator;

typedef WriterBufferingIterator DefaultWritingIterator;

template <typename ITERATOR = DefaultWritingIterator>
class Writer
{
	enum { DEFAULT_ROWS_BUFFER_SIZE = 10000 };
public:
	typedef ITERATOR iterator_class;
	typedef IteratorProxy<ITERATOR, Writer> iterator;

	Writer(const PathName path);
	Writer(DataHandle &, bool openDataHandle=true);
	Writer(DataHandle *, bool openDataHandle=true, bool deleteDataHandle=false);
	Writer();
	virtual ~Writer();

	iterator begin(bool openDataHandle=true);

	DataHandle& dataHandle() { return *dataHandle_; };

	ITERATOR* createWriteIterator(PathName, bool append = false);

	unsigned long rowsBufferSize() { return rowsBufferSize_; }
	Writer& rowsBufferSize(unsigned long n) { rowsBufferSize_ = n; }

protected:
	ITERATOR* writer(bool fixedSizeRows = false);

private:
// No copy allowed
    Writer(const Writer&);
    Writer& operator=(const Writer&);

	const PathName path_;
	DataHandle* dataHandle_;
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
