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

#include "ecml/parser/Request.h"

#include "ecml/core/RequestHandler.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"
#include "ecml/core/SpecialFormHandler.h"

#include "odb_api/ecml_verbs/ArchiveHandler.h"
#include "odb_api/ecml_verbs/RetrieveHandler.h"
#include "odb_api/ecml_verbs/StageHandler.h"
#include "odb_api/ecml_verbs/SQLHandler.h"
#include "odb_api/ecml_verbs/CompareHandler.h"
#include "odb_api/ecml_verbs/ChunkHandler.h"
#include "odb_api/ecml_verbs/SQLTestHandler.h"
#include "odb_api/ecml_verbs/CreatePartitionsHandler.h"
#include "odb_api/ecml_verbs/CreateIndexHandler.h"
#include "odb_api/ecml_verbs/ImportTextHandler.h"

#include "ODBModule.h"

namespace odb {

using namespace std;
using namespace ecml;
using namespace eckit;

ODBModule::ODBModule() {}
ODBModule::~ODBModule() {}

void ODBModule::importInto(ExecutionContext& context)
{
    static ArchiveHandler archive("odb.archive");
    static RetrieveHandler retrieve("odb.retrieve", false);
    static RetrieveHandler local_retrieve("odb.local_retrieve", true);
    static StageHandler stage("odb.stage", false);
    static StageHandler local_stage("odb.local_stage", true);
    static SQLHandler sql("odb.sql");
    static SQLHandler split("odb.split");
    static CompareHandler compare("odb.compare");
    static ChunkHandler chunk("odb.chunk");
    static SQLTestHandler sql_test("odb.sql_test");
    static CreatePartitionsHandler create_partitions("odb.create_partitions");
    static CreateIndexHandler create_index("odb.create_index");
    static ImportTextHandler import_text("odb.import_text");

    context.registerHandler("archive", archive);
    context.registerHandler("retrieve", retrieve);
    context.registerHandler("local_retrieve", local_retrieve);
    context.registerHandler("stage", stage);
    context.registerHandler("local_stage", local_stage);
    context.registerHandler("sql", sql);
    context.registerHandler("split", split);
    context.registerHandler("compare", compare);
    context.registerHandler("chunk", chunk);
    context.registerHandler("sql_test", sql_test);
    context.registerHandler("create_partitions", create_partitions);
    context.registerHandler("create_index", create_index);
    context.registerHandler("import_text", import_text);
}

} // namespace odb 
