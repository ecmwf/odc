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

#include "eckit/parser/Request.h"

#include "eckit/utils/RequestHandler.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"

#include "ArchiveHandler.h"
#include "RetrieveHandler.h"
#include "SQLHandler.h"
#include "CompareHandler.h"
#include "ListHandler.h"
#include "VariableLookupHandler.h"
#include "PrintHandler.h"

#include "ODBModule.h"

using namespace std;

ODBModule::ODBModule() {}
ODBModule::~ODBModule() {}

static Request native(const string& name)
{
    return new Cell("_native", name, 0, 0);
}

void ODBModule::importInto(ExecutionContext& context)
{
    static ArchiveHandler archive("odb.archive");
    static RetrieveHandler retrieve("odb.retrieve");
    static SQLHandler sql("odb.sql");
    static SQLHandler split("odb.split");
    static CompareHandler compare("odb.compare");
    static ListHandler list("list");
    static VariableLookupHandler value("value");
    static PrintHandler print("print", "");
    static PrintHandler printnl("printnl", "\n");

    Environment& e(context.environment());
    e.set("archive", native(archive.name()));
    e.set("retrieve", native(retrieve.name())); 
    e.set("sql", native(sql.name()));
    e.set("split", native(split.name()));
    e.set("compare", native(compare.name()));
    e.set("list", native(list.name()));
    e.set("value", native(value.name()));
    e.set("print", native(print.name()));
    e.set("printnl", native(printnl.name()));
}

