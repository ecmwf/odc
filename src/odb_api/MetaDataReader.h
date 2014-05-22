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
/// \file MetaDataReader.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef MetaDataReader_H
#define MetaDataReader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "odb_api/IteratorProxy.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

namespace odb {

class MetaDataReaderIterator;

template <typename T>
class MetaDataReader
{
public:
	typedef IteratorProxy<T,MetaDataReader,const double> iterator;
	//typedef typename iterator::Row row;

	MetaDataReader(const eckit::PathName &path, bool skipData = true);
	MetaDataReader();

	virtual ~MetaDataReader();

	iterator begin();
	const iterator end(); 

    eckit::DataHandle& dataHandle() { return *dataHandle_; }
    
	// For C API
	iterator* createReadIterator(const eckit::PathName&);

#ifdef SWIGPYTHON
	iterator __iter__() { return begin(); }
#endif

private:
// No copy allowed
    MetaDataReader(const MetaDataReader&);
    MetaDataReader& operator=(const MetaDataReader&);

	eckit::DataHandle* dataHandle_;
	bool deleteDataHandle_;
	//const eckit::PathName path_;
	const std::string path_;
	bool skipData_;

	friend class IteratorProxy<MetaDataReaderIterator,MetaDataReader,const double>;
	friend class MetaDataReaderIterator;
};

} // namespace odb

#include "odb_api/MetaDataReader.cc"

#endif
