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

#include "eckit/utils/RequestHandler.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"

#include "ArchiveHandler.h"
#include "RetrieveHandler.h"
#include "SQLHandler.h"
#include "CompareHandler.h"
#include "DuplicateHandler.h"
#include "ListHandler.h"

#include "ODBModule.h"

ODBModule::ODBModule() {}
ODBModule::~ODBModule() {}

void ODBModule::importInto(ExecutionContext& context)
{
    Environment& e(context.environment());
    e.set("archive", new ArchiveHandler("archive"));
    e.set("retrieve", new RetrieveHandler("retrieve"));
    e.set("sql", new SQLHandler("sql"));
    e.set("split", new SQLHandler("split"));
    e.set("compare", new CompareHandler("compare"));
    e.set("duplicate", new DuplicateHandler("duplicate"));
    e.set("list", new ListHandler("list"));
}

