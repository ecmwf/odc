/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Odb2NetcdfModule.h
// Piotr Kuchta - (c) ECMWF May 2015

#ifndef Odb2NetcdfModule_H
#define Odb2NetcdfModule_H

#include "ecml/core/Module.h"
#include "ecml/core/ExecutionContext.h"

namespace odb {

class Odb2NetcdfModule : public ecml::Module {
public:
    Odb2NetcdfModule();
    ~Odb2NetcdfModule();
    void importInto(ecml::ExecutionContext&);
};

} // namespace odb

#endif
