/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ODBMigratorModule.h
// Piotr Kuchta - (c) ECMWF August 2015

#ifndef eckit_ecml_ODBMigratorModule_H
#define eckit_ecml_ODBMigratorModule_H

#include "ecml/core/Module.h"
#include "ecml/core/ExecutionContext.h"

namespace odb {

class ODBMigratorModule : public ecml::Module {
public:
    ODBMigratorModule();
    ~ODBMigratorModule();
    void importInto(ecml::ExecutionContext&);
};

} // namespace odb

#endif
