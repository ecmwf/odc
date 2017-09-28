/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/FileHandle.h"

#include "ecml/parser/Request.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"

#include "odb_api/odb_api.h"
#include "odb_api/Comparator.h"
#include "odb_api/tools/ImportTool.h"
#include "odb_api/SQLInteractiveSession.h"

#include "SQLTestHandler.h"
#include "ImportTextHandler.h"

using namespace std;
using namespace eckit;

namespace odb {

SQLTestHandler::SQLTestHandler(const string& name)
: RequestHandler(name)
{
}

PathName SQLTestHandler::generatePathName(const string& testLabel, const string& parameterName)
{
   return testLabel + "_" + parameterName + ".odb";
}


PathName SQLTestHandler::write(const string& testLabel, const string& parameterName, const string& csv)
{
    const PathName fn (generatePathName(testLabel, parameterName));
    ImportTextHandler::importText(fn, csv);
    return fn;
}

void SQLTestHandler::createInputTables(ecml::Cell* dictionary)
{
    if (! dictionary) return;
    dictionary = dictionary->value();
    if (! dictionary) return;

    for (ecml::Cell* p (dictionary->rest()); p; p = p->rest())
    {
        string tableName (p->text());
        ecml::Cell* csv (p->value()->value());
        const string text (csv->text());

        Log::info() << "createInputTables: tableName: " << tableName << endl;
        Log::info() << "createInputTables: text: " << text << endl;

        ImportTextHandler::importText(PathName(tableName + ".odb"), text);
    }
}

/// Accepted parameters: label, input, expect, sql
ecml::Request SQLTestHandler::handle(ecml::ExecutionContext& context)
{
    ecml::Environment& e (context.environment());

    const string label (e.lookup("label", "", context));
    const string input (e.lookup("input", "", context));
    const string expect (e.lookup("expect", "", context));
    const string sql (e.lookup("sql", "", context));

    ecml::Cell * inputTables (e.lookupNoThrow("input_tables"));
    createInputTables (inputTables);

    if (! (expect.size() && sql.size()))
        throw UserError("sql_test: you need to pass parameters sql, expect and input or input_tables");

    PathName expectedOutput (write(label, "expect", expect));
    PathName actualOutput (generatePathName(label, "actual_output"));

    odb::Writer<> writer (actualOutput);
    odb::Writer<>::iterator output (writer.begin());

    if (input.size())
    {
        PathName in (write(label, "input", input));
        odb::Select select (sql, in);
        unsigned long long rowsProduced (output->pass1(select.begin(), select.end()));
    } 
    else 
    {
        odb::Select select (sql);
        unsigned long long rowsProduced (output->pass1(select.begin(), select.end()));
    }

    FileHandle l (expectedOutput);
    FileHandle r (actualOutput);
    l.openForRead();
    r.openForRead();

    odb::Comparator comparator;
    comparator.compare (l, r);

    ecml::List result;
    result.append(actualOutput);

    return result;
}

} // namespace odb 

