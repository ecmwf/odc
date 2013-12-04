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
/// \file DirectAccess.h
///
/// @author Baudouin Raoult, Dec 2013


#ifndef DirectAccess_H
#define DirectAccess_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "odblib/IteratorProxy.h"
#include "odblib/DirectAccessIterator.h"


namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

namespace odb {

class DirectAccessIterator;

class DirectAccess
{
public:
    typedef IteratorProxy<DirectAccessIterator, DirectAccess, const double> iterator;
	typedef iterator::Row row;

	DirectAccess(eckit::DataHandle &);
    DirectAccess(const std::string& path);
	DirectAccess();

	virtual ~DirectAccess();

	iterator begin();
	const iterator end(); 

	eckit::DataHandle* dataHandle() { return dataHandle_; }
	// For C API
	DirectAccessIterator* createReadIterator(const eckit::PathName&);
	DirectAccessIterator* createReadIterator();

#ifdef SWIGPYTHON
	iterator __iter__() { return iterator(createReadIterator()); }
#endif

private:
// No copy allowed
    DirectAccess(const DirectAccess&);
    DirectAccess& operator=(const DirectAccess&);


	eckit::DataHandle* dataHandle_;
	bool deleteDataHandle_;
	//const eckit::PathName path_;
	const std::string path_;

	friend class IteratorProxy<DirectAccessIterator,DirectAccess,const double>;
	friend class DirectAccessIterator;
};

} // namespace odb

#endif
