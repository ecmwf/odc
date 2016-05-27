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

#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/MultiHandle.h"

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"

#include "odb_api/odb_api.h"
#include "odb_api/StringTool.h"

#include "ecml/data/DataHandleFactory.h"

#include "MigrateHandler.h"
#include "migrator_api.h"

using namespace std;
using namespace eckit;
using namespace odb;
using namespace odb::sql;

MigrateHandler::MigrateHandler(const string& name) : RequestHandler(name) {}

ecml::Values MigrateHandler::handle(ecml::ExecutionContext& context)
{
    string target (context.environment().lookup("target", "", context)),
           filter (cleanUpSQLText(context.environment().lookup("filter", "", context))),
           source (context.environment().lookup("source", "", context));

    if (! source.size()) throw UserError("SOURCE is obligatory");
    if (! target.size()) throw UserError("TARGET is obligatory");

    Log::debug() << "MigrateHandler:" << " target: " << target << ", source: " << source << ", filter: " << filter << endl;


    TmpFile sqlFile;
    ofstream f (sqlFile.asString().c_str());
    f << filter;
    f.close();

    int rc = 0;
    if ((rc = odb::tool::import_odb_with_sql_in_file(source.c_str(),
                                          sqlFile.asString().c_str(),
                                          target.c_str())))
    {
        throw UserError("migrator failed");
    }

    ecml::List r;
    r.append(target);

    return r;
}

string MigrateHandler::cleanUpSQLText(const string& sql)
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

