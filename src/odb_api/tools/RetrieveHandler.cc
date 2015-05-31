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

#include "RetrieveHandler.h"
#include "SQLHandler.h"

#include "eckit/io/MultiHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/parser/Request.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"
#include "eckit/utils/DataHandleFactory.h"

#include "odb_api/DispatchingWriter.h"
#include "odb_api/TemplateParameters.h"

using namespace std;
using namespace eckit;
using namespace odb;

RetrieveHandler::RetrieveHandler(const string& name) : RequestHandler(name) {}

Values RetrieveHandler::handle(ExecutionContext& context)
{
    Request request (context.environment().currentFrame());
    //request->showGraph(false);
    const string host (database(context)),
                 target (context.environment().lookup("target", "")),
                 filter (context.environment().lookup("filter", ""));

    if (! target.size())
        throw UserError("You must specify TARGET explicitly");

    Log::info() << "RETRIEVE from " << host << " into " << target << endl;

    stringstream ss;
    ss << request;
    vector<string> v;
    v.push_back(ss.str());
    MultiHandle input;
    DataHandleFactory::buildMultiHandle(input, v);
    Log::info() << "RETRIEVE input " << input << endl;

    TemplateParameters templateParameters;
    TemplateParameters::parse(target, templateParameters);

    vector<PathName> r;
    if (filter.size() || templateParameters.size())
    {
        try { 
            r = SQLHandler::executeSelect(filter, input, target);
        } catch (eckit::ReadError e) {
            Log::warning() << "No data received from " << host << endl;
        }
    }
    else
    {
        FileHandle p(target);
        input.saveInto(p);
        r.push_back(target);
    }

    ASSERT(r.size());

    Values rv(0);
    List list(rv);
    for (size_t i(0); i < r.size(); ++i)
        list.append(r[i]);

    ASSERT(rv);
    return rv;
}

