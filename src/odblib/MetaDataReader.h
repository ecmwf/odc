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

class MetaDataReader
{
public:
	typedef IteratorProxy<MetaDataReaderIterator,MetaDataReader,const double> iterator;
	typedef iterator::Row row;

	MetaDataReader(const PathName& path);
	MetaDataReader();

	virtual ~MetaDataReader();

	iterator begin();
	const iterator end(); 

	DataHandle* dataHandle() { return dataHandle_; };
	// For C API
	MetaDataReaderIterator* createReadIterator(const PathName&);

#ifdef SWIGPYTHON
	iterator __iter__() { return begin(); }
#endif

private:
// No copy allowed
    MetaDataReader(const MetaDataReader&);
    MetaDataReader& operator=(const MetaDataReader&);

    DataHandle* dataHandle_;
	bool deleteDataHandle_;
	//const PathName path_;
	const string path_;

	friend class IteratorProxy<MetaDataReaderIterator,MetaDataReader,const double>;
	friend class MetaDataReaderIterator;
};

} // namespace odb

#endif
