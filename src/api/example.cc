/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file examples.cc
///
/// This file contains examples of usage of public APIs.
///
/// @author Piotr Kuchta, ECMWF, July 2016

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "sqlite3.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/runtime/ContextBehavior.h"
#include "eckit/runtime/Context.h"

#include "odb_api/ODBBehavior.h"
#include "odb_api/tools/Tool.h"
#include "odb_api/tools/TestRunnerApplication.h"
#include "odb_api/tools/TestCase.h"
#include "odb_api/tools/ImportTool.h"

void prepareData()
{
    const char *data ("x:INTEGER,y:INTEGER,v:DOUBLE\n" "1,1,0.3\n" "1,1,0.2\n" "2,2,0.4\n" "2,2,0.1\n");
    odb::tool::ImportTool::importText(data, "example_select_data_read_results.odb");
}

void checkRC(int rc, const char* message, sqlite3 *db) 
{
    if (rc != SQLITE_OK) {
        std::string msg (std::string(message) + sqlite3_errmsg(db));
        sqlite3_close(db);
        throw eckit::AssertionFailed (msg);
    }
}

TEST(example_libversion)
{
    std::stringstream ss;
    ss << sqlite3_libversion();

}

TEST(example_select_data_read_results)
{
    // Prepare input data
    prepareData();

    sqlite3 *db;
    sqlite3_stmt *res;
    
    int rc = sqlite3_open(":memory:", &db);
    checkRC(rc, "Cannot open database: ", db);
    
    rc = sqlite3_prepare_v2(db, 
        //"SELECT SQLITE_VERSION()", 
        "SELECT '0.12.0';", 
        -1, 
        &res, 
        0);    
    checkRC(rc, "Failed to fetch data: ", db);
    
    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) {
        printf("%s\n", sqlite3_column_text(res, 0));
    }
    
    sqlite3_finalize(res);
    sqlite3_close(db);
}


TEST(example_read_data)
{
}

TEST(example_write_data)
{
}

int main(int argc, char** argv)
{
    using namespace odb;
    using namespace odb::tool;
    using namespace eckit;

	Tool::registerTools();
    static ContextBehavior* behavior (0);
    if (behavior == 0)
        Context::instance().behavior(behavior = new ODBBehavior());
    odb::tool::test::TestRunnerApplication testRunner(argc, argv);
    testRunner.start();
}


