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

#include "ImportTextHandler.h"

using namespace std;
using namespace eckit;

namespace odb {

ImportTextHandler::ImportTextHandler(const string& name)
: RequestHandler(name)
{}

unsigned long long ImportTextHandler::importText(const PathName& pathName, const string& csv)
{
    const string delimiter(",");
    odb::sql::SQLInteractiveSession session;
	session.selectFactory().csvDelimiter(delimiter);
	stringstream fs(csv);
	odb::Select input ("select *;", fs, delimiter);
	odb::Writer<> writer (pathName);
	odb::Writer<>::iterator output (writer.begin());

	unsigned long long n (output->pass1(input.begin(), input.end()));
    return n;
}

/// Accepted parameters: text, source, target
ecml::Request ImportTextHandler::handle(ecml::ExecutionContext& context)
{
    ecml::Environment& e (context.environment());

    //const string source (e.lookup("source", "", context));
    const PathName target (e.lookup("target", "", context));
    vector<string> texts (context.environment().lookupList("text", context));

    if (! texts.size())
        throw UserError("import_text: missing parameter TEXT");

    if (texts.size() != 1)
        throw UserError("import_text: multiple values of TEXT not yet supported");

    if (! string(target).size())
        throw UserError("import_text: obligatory parameter TARGET must be a single path name");

    for (size_t i(0); i < texts.size(); ++i)
    {
        const string text (texts[i]);
        importText(target, text);
    }

    ecml::List l;
    l.append(target);

    return l;
}

} // namespace odb 

