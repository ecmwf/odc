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

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"

#include "odb_api/odb_api.h"
#include "odb_api/StringTool.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLParser.h"
#include "odb_api/SQLSelectFactory.h"
#include "odb_api/SQLSelect.h"

#include "ecml/data/DataHandleFactory.h"

using namespace std;
using namespace eckit;
using namespace ecml;
using namespace odb;
using namespace odb::sql;

namespace odb {

SQLHandler::SQLHandler(const string& name) : RequestHandler(name) {}

Values SQLHandler::handle(ecml::ExecutionContext& context)
{
    string target (context.environment().lookup("target", "", context)),
           include (cleanUpSQLText(context.environment().lookup("include", "", context)));
           //callback (context.environment().lookup("callback", "", context));

    vector<string> filters (context.getValueAsList("filter"));
    vector<string> sources (context.getValueAsList("source"));
    if (sources.empty())
        throw UserError("sql: SOURCE is empty");

    string filter;
    for (size_t i(0); i < filters.size(); ++i)
        filter += cleanUpSQLText(filters[i]) + "\n";

    MultiHandle input;
    DataHandleFactory::buildMultiHandle(input, sources);

    Log::debug() << "SQLHandler:" << " target: " << target << ", input : " << input << ", filter: " << filter << endl;

    vector<string> ps( pathNamesToStrings(executeSelect(filter, include, input, target, &context)) );
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
    return executeSelect(select, "", input, into, context);
}

vector<PathName> SQLHandler::executeSelect(const string& select, const string& inc, DataHandle& input, const string& into, ExecutionContext* context)
{
    // We don't call saveInto here so have to open the handle explicitly:
    input.openForRead();

    string sql(select);
    if (! sql.size())
    {
        Log::info() << "No SQL, using default 'select *;'" << endl;
        sql = "select *;";
    }

    sql = cleanUpSQLText(sql);
    Log::info() << "Executing '" << sql << "'" << endl;

    SQLNonInteractiveSession session;

    SQLOutputConfig config(session.selectFactory().config());
    config.outputFormat("odb");
    config.outputFile(into);

    SQLParser parser;
    if (inc.size())
        parser.parseString(session, inc, &input, config);
    parser.parseString(session, sql, &input, config);

    SQLStatement* statement (session.statement());
    if (! statement)
    {
        //throw UserError("sql: No SELECT parsed");
        Log::info()  << "sql: No SELECT parsed" << endl;
        return vector<PathName> ();
    }

    SQLSelect* sqlSelect(dynamic_cast<SQLSelect*>(statement));
    if (! sqlSelect) 
    {
        stringstream ss;
        ss << "sql: Parsed statement " << *statement << " is not SELECT";
        throw UserError(ss.str());
    }

    long long numberOfRows (sqlSelect->execute(context));
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

} // namespace odb 

