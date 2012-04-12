///
/// \file MetaDataReader.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef MetaDataReader_H
#define MetaDataReader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

class PathName;
class DataHandle;

namespace odb {

class MetaDataReaderIterator;

template <typename T>
class MetaDataReader
{
public:
	typedef IteratorProxy<T,MetaDataReader,const double> iterator;
	//typedef typename iterator::Row row;

	MetaDataReader(const std::string& path, bool skipData = true);
	MetaDataReader();

	virtual ~MetaDataReader();

	iterator begin();
	const iterator end(); 

	FileHandle* dataHandle() { return dataHandle_; };
	// For C API
	iterator* createReadIterator(const PathName&);

#ifdef SWIGPYTHON
	iterator __iter__() { return begin(); }
#endif

private:
// No copy allowed
    MetaDataReader(const MetaDataReader&);
    MetaDataReader& operator=(const MetaDataReader&);

	FileHandle* dataHandle_;
	bool deleteDataHandle_;
	//const PathName path_;
	const string path_;
	bool skipData_;

	friend class IteratorProxy<MetaDataReaderIterator,MetaDataReader,const double>;
	friend class MetaDataReaderIterator;
};

} // namespace odb

#include "MetaDataReader.cc"

#endif
