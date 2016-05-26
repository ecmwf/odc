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

#ifndef odb_api_LetHandler_H
#define odb_api_LetHandler_H

#include "eckit/filesystem/PathName.h"

#include "ecml/parser/Request.h"
#include "ecml/SpecialFormHandler.h"

namespace eckit { class ExecutionContext; }

namespace odb { 
namespace tool {  

class LetHandler : public ecml::SpecialFormHandler {
public:
    LetHandler(const std::string&);

    virtual ecml::Request handle(const ecml::Request, ecml::ExecutionContext&);
};

} //namespace tool
} // namespace odb

#endif
