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

#include "odb_api/IteratorProxy.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLIteratorSession.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }
namespace ecml { class ExecutionContext; }

namespace odb {

class SelectIterator;

class Select
{
public:
	typedef IteratorProxy<SelectIterator,Select,const double> iterator;
	typedef iterator::Row row;

	Select(const std::string& selectStatement, eckit::DataHandle &);
	Select(const std::string& selectStatement, eckit::DataHandle &, ecml::ExecutionContext*);
	Select(const std::string& selectStatement, std::istream &, const std::string& delimiter);
	Select(const std::string& selectStatement, std::istream &, const std::string& delimiter, ecml::ExecutionContext*);
    Select(const std::string& selectStatement, const std::string& path);
    Select(const std::string& selectStatement, const std::string& path, ecml::ExecutionContext*);
	Select(const std::string& selectStatement);
	Select(const std::string& selectStatement, ecml::ExecutionContext*);
	Select(const std::string& selectStatement, odb::sql::SQLNonInteractiveSession&);
	Select();
	Select(ecml::ExecutionContext*);

	virtual ~Select();

#ifdef SWIGPYTHON
	iterator __iter__() { return iterator(createSelectIterator(selectStatement_, context_)); }
#endif

	iterator begin();
	const iterator end();

	eckit::DataHandle* dataHandle() { return dataHandle_; };
	std::istream* dataIStream() { return istream_; }

	SelectIterator* createSelectIterator(const std::string&, ecml::ExecutionContext*);

private:
    odb::sql::SQLNonInteractiveSession* ownSession(const std::string& delimiter);

	friend class odb::IteratorProxy<odb::SelectIterator, odb::Select, const double>;

	eckit::DataHandle* dataHandle_;
	bool deleteDataHandle_;

	std::istream* istream_;
	bool deleteIStream_;

	std::string selectStatement_;
	std::string delimiter_;

    ecml::ExecutionContext* context_;
    odb::sql::SQLNonInteractiveSession* ownSession_;
    odb::sql::SQLNonInteractiveSession* outerSession_;
};

} // namespace odb 

#endif
