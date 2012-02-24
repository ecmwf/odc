///
/// \file TextReader.h
///
/// @author Piotr Kuchta, Oct 2010

#ifndef TextReader_H
#define TextReader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

class PathName;

namespace odb {

class TextReaderIterator;

class TextReader
{
public:
	typedef IteratorProxy<TextReaderIterator,TextReader,const double> iterator;
	typedef iterator::Row row;

	TextReader(istream &);
	TextReader(const std::string& path);
	TextReader();

	virtual ~TextReader();

	iterator begin();
	const iterator end(); 

	istream& stream() { return *in_; }
	// For C API
	TextReaderIterator* createReadIterator(const PathName&);

#ifdef SWIGPYTHON
	iterator __iter__() { return begin(); }
#endif

private:
// No copy allowed
    TextReader(const TextReader&);
    TextReader& operator=(const TextReader&);

	istream* in_;
	bool deleteDataHandle_;
	//const PathName path_;
	const string path_;

	friend class IteratorProxy<TextReaderIterator,TextReader,const double>;
	friend class TextReaderIterator;
};

} // namespace odb

#endif
