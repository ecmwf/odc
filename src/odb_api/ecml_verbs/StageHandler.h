/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, November 2015

#ifndef odb_api_StageHandler_H
#define odb_api_StageHandler_H

#include <sstream>

#include "ecml/parser/Request.h"
#include "ecml/core/RequestHandler.h"

namespace odb {

class StageHandler : public ecml::RequestHandler {
public:
    StageHandler(const std::string&, bool local);

    virtual eckit::Values handle(ecml::ExecutionContext&);

private:
    bool local_;
};

} // namespace odb

#endif
