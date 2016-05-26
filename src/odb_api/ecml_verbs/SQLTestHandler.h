/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, October 2015

#ifndef odb_api_SQLTestHandler_H
#define odb_api_SQLTestHandler_H

#include "eckit/filesystem/PathName.h"

#include "ecml/parser/Request.h"
#include "ecml/core/RequestHandler.h"

namespace odb {

class SQLTestHandler : public ecml::RequestHandler {
public:
    SQLTestHandler(const std::string&);

private:
    eckit::PathName generatePathName(const std::string& testLabel, const std::string& parameterName);
    eckit::PathName write(const std::string& testLabel, const std::string& parameterName, const std::string& csv);
    void createInputTables(eckit::Cell*);

    virtual eckit::Values handle(ecml::ExecutionContext&);
};

} // namespace odb

#endif
