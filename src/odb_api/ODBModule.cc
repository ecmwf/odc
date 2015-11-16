/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ODBModule.cc
// Piotr Kuchta - (c) ECMWF May 2015

#include <string>

#include "eckit/ecml/parser/Request.h"

#include "eckit/ecml/core/RequestHandler.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/SpecialFormHandler.h"

#include "odb_api/ecml_verbs/ArchiveHandler.h"
#include "odb_api/ecml_verbs/RetrieveHandler.h"
#include "odb_api/ecml_verbs/SQLHandler.h"
#include "odb_api/ecml_verbs/CompareHandler.h"

#include "ODBModule.h"

using namespace std;
using namespace eckit;

namespace odb {

ODBModule::ODBModule() {}
ODBModule::~ODBModule() {}

static Request native(const string& name) { return new Cell("_native", name, 0, 0); }
static Request macro(const string& name) { return new Cell("_macro", name, 0, 0); }

void ODBModule::importInto(ExecutionContext& context)
{
    static ArchiveHandler archive("odb.archive");
    static RetrieveHandler retrieve("odb.retrieve");
    static SQLHandler sql("odb.sql");
    static SQLHandler split("odb.split");
    static CompareHandler compare("odb.compare");

    Environment& e(context.environment());
    e.set("archive", native(archive.name()));
    e.set("retrieve", native(retrieve.name())); 
    e.set("sql", native(sql.name()));
    e.set("split", native(split.name()));
    e.set("compare", native(compare.name()));
}

}  // namespace odb
