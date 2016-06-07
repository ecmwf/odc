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
/// \file Reader.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef Reader_H
#define Reader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "odb_api/IteratorProxy.h"
#include "odb_api/ReaderIterator.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }
namespace ecml { class ExecutionContext; }

namespace odb {

class Reader
{
public:
	typedef IteratorProxy<ReaderIterator,Reader,const double> iterator;
	typedef iterator::Row row;

	Reader(eckit::DataHandle &);
	Reader(eckit::DataHandle &, ecml::ExecutionContext*);
    Reader(const std::string& path);
    Reader(const std::string& path, ecml::ExecutionContext*);
	Reader();
	Reader(ecml::ExecutionContext*);

	virtual ~Reader();

	iterator begin();
	const iterator end(); 

	eckit::DataHandle* dataHandle() { return dataHandle_; }
	// For C API
	ReaderIterator* createReadIterator(const eckit::PathName&);
	ReaderIterator* createReadIterator();

#ifdef SWIGPYTHON
	iterator __iter__() { return iterator(createReadIterator()); }
#endif

private:
// No copy allowed
    Reader(const Reader&);
    Reader& operator=(const Reader&);

	eckit::DataHandle* dataHandle_;
	bool deleteDataHandle_;
	//const eckit::PathName path_;
	const std::string path_;
    ecml::ExecutionContext* context_;

	friend class IteratorProxy<ReaderIterator,Reader,const double>;
	friend class ReaderIterator;
};

} // namespace odb

#endif
