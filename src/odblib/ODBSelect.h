///
/// \file Select.h
///
/// @author Piotr Kuchta, April 2010

#ifndef ODBSELECT_H
#define ODBSELECT_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

class PathName;
class DataHandle;

namespace odb {

class Reader;
class HashTable;
class SQLIteratorSession;
class RowsReaderIterator;
class ReaderIterator;
class WriterBufferingIterator;
//class Writer;
class SelectIterator;

class Select
{
public:
	typedef IteratorProxy<SelectIterator,Select,const double> iterator;
	typedef iterator::Row row;

	Select(const string& selectStatement, DataHandle &);
	Select(const string& selectStatement, std::istream &);
	Select(const string& selectStatement, const std::string& path);
	Select(const string& selectStatement);
	Select();

	virtual ~Select();

#ifdef SWIGPYTHON
	iterator __iter__() { return iterator(createSelectIterator(selectStatement_)); }
#endif

	iterator begin();
	const iterator end();

	DataHandle* dataHandle() { return dataHandle_; };
	std::istream* dataIStream() { return istream_; }

	SelectIterator* createSelectIterator(string);

private:
	friend class IteratorProxy<SelectIterator,Select,const double>;

	DataHandle* dataHandle_;
	bool deleteDataHandle_;

	std::istream* istream_;
	bool deleteIStream_;

	string selectStatement_;
};

} // namespace odb 

#endif
