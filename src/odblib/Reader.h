///
/// \file Reader.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef Reader_H
#define Reader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

class PathName;
class DataHandle;

namespace odb {

class ReaderIterator;

class Reader
{
public:
	typedef IteratorProxy<ReaderIterator,Reader,const double> iterator;
	typedef iterator::Row row;

	Reader(DataHandle &);
	Reader(const std::string& path);
	Reader();

	virtual ~Reader();

	iterator begin();
	const iterator end(); 

	DataHandle* dataHandle() { return dataHandle_; };
	// For C API
	ReaderIterator* createReadIterator(const PathName&);
	ReaderIterator* createReadIterator();

#ifdef SWIGPYTHON
	iterator __iter__() { return iterator(createReadIterator()); }
#endif

private:
// No copy allowed
    Reader(const Reader&);
    Reader& operator=(const Reader&);


	DataHandle* dataHandle_;
	bool deleteDataHandle_;
	//const PathName path_;
	const string path_;

	friend class IteratorProxy<ReaderIterator,Reader,const double>;
	friend class ReaderIterator;
};

} // namespace odb

#endif
