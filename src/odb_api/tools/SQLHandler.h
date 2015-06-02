/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef SQLHandler_H
#define SQLHandler_H

#include "eckit/parser/Request.h"
#include "eckit/utils/RequestHandler.h"

class SQLHandler : public eckit::RequestHandler {
public:
    SQLHandler(const std::string&);
    virtual Values handle(eckit::ExecutionContext&);

    static std::vector<eckit::PathName> executeSelect(const std::string&, eckit::DataHandle&, const std::string&);
    static std::string cleanUpSQLText(const std::string&);
};

#endif
