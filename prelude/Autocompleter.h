/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, May 2016

#ifndef eckit_ecml_Autocompleter_H
#define eckit_ecml_Autocompleter_H

#include "ecml/parser/Request.h"
#include "ecml/core/ExecutionContext.h"

namespace ecml {

class Autocompleter {

private:
    static bool completion(const char*, int, char*, int);
    static void describe(const std::string&, eckit::Cell*);
    static bool notInWord(char);
    static std::string suffix(const char* line, int pos);

    static ExecutionContext* context_;

    friend class REPLHandler;
};

} // namespace ecml


#endif
