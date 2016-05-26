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

#ifndef eckit_ecml_MigrateHandler_H
#define eckit_ecml_MigrateHandler_H

#include <string>

#include "ecml/parser/Request.h"
#include "ecml/core/RequestHandler.h"

#include "ecml/core/ExecutionContext.h"

class MigrateHandler : public ecml::RequestHandler {
public:
    MigrateHandler(const std::string&);
    virtual eckit::Values handle(ecml::ExecutionContext&);
private:
    std::string cleanUpSQLText(const std::string&);
};

#endif
