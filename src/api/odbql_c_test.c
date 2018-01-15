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

int main(int argc, char** argv) {
    if (odbql_example_insert_data()) 
        return fprintf(stderr, "odbql_example_insert_data FAILED\n"), 1;
    
    if (odbql_example_select_data_read_results()) 
        return fprintf(stderr, "odbql_example_select_data_read_results FAILED\n"), 1;

    fprintf(stdout, "%s: All done.\n", argv[0]);
    return 0;
}

