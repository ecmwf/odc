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
/// \file Select.h
///
/// @author Piotr Kuchta, April 2010

#ifndef ODBSELECT_H
#define ODBSELECT_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "odblib/IteratorProxy.h"

namespace eclib { class PathName; }
namespace eclib { class DataHandle; }

namespace odb {

class SelectIterator;

class Select
{
public:
	typedef IteratorProxy<SelectIterator,Select,const double> iterator;
	typedef iterator::Row row;

	Select(const string& selectStatement, eclib::DataHandle &);
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

	eclib::DataHandle* dataHandle() { return dataHandle_; };
	std::istream* dataIStream() { return istream_; }

        SelectIterator* createSelectIterator(string);

private:
	friend class odb::IteratorProxy<odb::SelectIterator, odb::Select, const double>;

	eclib::DataHandle* dataHandle_;
	bool deleteDataHandle_;

	std::istream* istream_;
	bool deleteIStream_;

	string selectStatement_;
};

} // namespace odb 

#endif
