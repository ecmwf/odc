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
/// \file CallbackExecutor.h
///
/// @author Piotr Kuchta, April 2015

#ifndef CallbackExecutor_h
#define CallbackExecutor_h

namespace eckit { class DataHandle; }

namespace odb {

namespace sql { class SQLRowCallback; class SQLRowsCallback; }

class CallbackExecutor
{
public:
	CallbackExecutor(const std::string& selectStatement, eckit::DataHandle &);
    CallbackExecutor(const std::string& selectStatement, const std::string& path);
	CallbackExecutor(const std::string& selectStatement);
	CallbackExecutor();

	virtual ~CallbackExecutor();

    void execute(odb::sql::SQLRowCallback&);

private:
	eckit::DataHandle* dataHandle_;
	bool deleteDataHandle_;

	std::string selectStatement_;
};

} // namespace odb 

#endif
