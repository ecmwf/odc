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
/// \file MetaDataReader.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef MetaDataReader_H
#define MetaDataReader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "odblib/IteratorProxy.h"

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
