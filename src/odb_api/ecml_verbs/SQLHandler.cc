/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "SQLHandler.h"

#include "eckit/io/MultiHandle.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/parser/RequestParser.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"

#include "odb_api/odb_api.h"
#include "odb_api/StringTool.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLParser.h"
#include "odb_api/SQLSelectFactory.h"
#include "odb_api/SQLSelect.h"

#include "experimental/eckit/ecml/data/DataHandleFactory.h"

using namespace std;
using namespace eckit;
using namespace odb;
using namespace odb::sql;

SQLHandler::SQLHandler(const string& name) : RequestHandler(name) {}

Values SQLHandler::handle(ExecutionContext& context)
{
    string target (context.environment().lookup("target", "", context)),
           filter (cleanUpSQLText(context.environment().lookup("filter", "", context)));
           //callback (context.environment().lookup("callback", "", context));
    vector<string> sources (getValueAsList(context, "source"));

    MultiHandle input;
    DataHandleFactory::buildMultiHandle(input, sources);

    Log::debug() << "SQLHandler:" << " target: " << target << ", input : " << input << ", filter: " << filter << endl;

    input.openForRead();

    vector<string> ps( pathNamesToStrings(executeSelect(filter, input, target, &context)) );
    //ASSERT(ps.size());
    Values vs(0);
    List list(vs);
    for (size_t i(0); i < ps.size(); ++i)
        list.append(ps[i]);

    //if (vs) vs->showGraph("SQLHandler::handle => " + vs->str());
    // TODO: return an empty list object?
    if (vs) return vs;
    else return new Cell("_list", "", 0, 0);
}

vector<PathName> SQLHandler::executeSelect(const string& select, DataHandle& input, const string& into, ExecutionContext* context)
{
    // We don't call saveInto here so have to open the handle explicitly:
    input.openForRead();

    string sql(select);
    if (! sql.size())
    {
        Log::info() << "No SQL, using default 'select *;'" << endl;
        sql = "select *;";
    }

    // TODO: append semicolon to filter if missing

    string s (cleanUpSQLText(sql));
    Log::info() << "Executing '" << s << "'" << endl;

    SQLNonInteractiveSession session;

    SQLOutputConfig config(SQLSelectFactory::instance().config());
    config.outputFormat("odb");
    config.outputFile(into);

    SQLParser parser;
    parser.parseString(sql, &input, config);

    SQLSelect* sqlSelect(dynamic_cast<SQLSelect*>(session.statement()));

    long long numberOfRows (sqlSelect->execute());
    vector<PathName> r(sqlSelect->outputFiles());

    delete sqlSelect;

    Log::info() << "Processed " << numberOfRows << " row(s)." << endl;

    return r;
}

string SQLHandler::cleanUpSQLText(const string& sql)
{
    if (sql.size() == 0)
        return sql;

    string s(sql);
    StringTool::trimInPlace(s);
    s = StringTool::isInQuotes(s) ? StringTool::unQuote(s) : s;
    StringTool::trimInPlace(s);
    if (s[s.size() - 1] != ';')
        s.append(";");
    return s;
}

