/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, December 2015

#ifndef eckit_ecml_ForHandler_H
#define eckit_ecml_ForHandler_H

#include "eckit/eckit_config.h"

#include "ecml/parser/Request.h"
#include "ecml/core/SpecialFormHandler.h"
#include "ecml/core/ExecutionContext.h"

namespace ecml {

class ForHandler : public SpecialFormHandler {
public:
    ForHandler(const std::string&);
    virtual ecml::Request handle(const ecml::Request, ExecutionContext&);

private:
    static ecml::Cell* exceptionValue(const std::string&);
};

} // namespace ecml


#endif
