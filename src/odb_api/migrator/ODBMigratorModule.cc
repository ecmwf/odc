/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ODBMigratorModule.cc
// Piotr Kuchta - (c) ECMWF May 2015

#include <string>

#include "ecml/parser/Request.h"
#include "ecml/core/RequestHandler.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"

#include "ODBMigratorModule.h"
#include "MigrateHandler.h"

namespace odb {

using namespace std;
using namespace eckit;
using namespace ecml;

ODBMigratorModule::ODBMigratorModule() {}
ODBMigratorModule::~ODBMigratorModule() {}

static Request native(const string& name) { return new Cell("_native", name, 0, 0); }

void ODBMigratorModule::importInto(ExecutionContext& context)
{
    static MigrateHandler migrate("odb.migrate");
    context.registerHandler("migrate", migrate);
}

} // namespace odb 
