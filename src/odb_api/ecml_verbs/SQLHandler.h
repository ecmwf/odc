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

#ifndef odb_api_SQLHandler_H
#define odb_api_SQLHandler_H

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/RequestHandler.h"

#include "experimental/eckit/ecml/core/ExecutionContext.h"

namespace odb {

class SQLHandler : public eckit::RequestHandler {
public:
    SQLHandler(const std::string&);
    virtual eckit::Values handle(eckit::ExecutionContext&);

    static std::vector<eckit::PathName> executeSelect(const std::string&, eckit::DataHandle&, const std::string&, eckit::ExecutionContext* context);

    static std::vector<eckit::PathName> executeSelect(const std::string&, const std::string&, eckit::DataHandle&, const std::string&, eckit::ExecutionContext* context);

    static std::string cleanUpSQLText(const std::string&);
};

} // namespace odb

#endif
