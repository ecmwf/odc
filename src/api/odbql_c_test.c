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

/*
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "odb_api/odbql.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/runtime/ContextBehavior.h"
#include "eckit/runtime/Context.h"

#include "odb_api/ODBBehavior.h"
#include "odb_api/tools/Tool.h"
#include "odb_api/tools/TestRunnerApplication.h"
#include "odb_api/tools/TestCase.h"
#include "odb_api/tools/ImportTool.h"
*/

#include <stdio.h>

/*
extern "C" {
*/

int odbql_example_insert_data();
int odbql_example_select_data_read_results();
/*
}
*/

int main() {
    if (0 == odbql_example_insert_data())
        fprintf(stderr, "odbql_example_insert_data passed OK\n");
    else {
        fprintf(stderr, "odbql_example_insert_data FAILED\n");
        return 1;
    }

    
    if (0 == odbql_example_select_data_read_results())
        fprintf(stderr, "odbql_example_select_data_read_results passed OK\n");
    else {
        fprintf(stderr, "odbql_example_select_data_read_results FAILED\n");
        return 1;
    }
}

/*
TEST(odbql_example_insert_data) { ASSERT(odbql_example_insert_data() == 0); }
TEST(odbql_example_select_data_read_results) { ASSERT(odbql_example_select_data_read_results() == 0); }

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
*/
