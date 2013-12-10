/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "migrator/ImportODBTool.h"
#include "migrator/migrator_api.h"
#include "odblib/odb_api.h"
#include "odblib/odbcapi.h"
#include "tools/Tool.h"


namespace odb {
namespace tool {

int import_odb_with_sql_in_file(const char* odb_database, const char* sql_file, const char* output_file)
{
    try {
        odb_start();
        const char *argv[] = {"importodb",  odb_database, sql_file, output_file, 0 };
        ImportODBTool<> importer(4, const_cast<char **>(argv));
        importer.run();
        return 0;
    } catch (...) {
        return 1;
    }
}


} // namespace tool 
} //namespace odb 


