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

#include "eckit/io/FileHandle.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/config/Resource.h"
#include "eckit/parser/StringTools.h"

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core/RequestHandler.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"

#include "odb_api/FileMapper.h"
#include "odb_api/FileCollector.h"
#include "odb_api/Archiver.h"
#include "odb_api/Stager.h"
#include "odb_api/Retriever.h"

#include "LocalHandleFactory.h"

using namespace eckit;
using namespace ecml;
using namespace std;

LocalHandleFactory::LocalHandleFactory()
: DataHandleFactory("local")
{}

DataHandle* LocalHandleFactory::makeHandle(const string& req) const
{
    const string& r (req);
    Log::info() << "LocalHandleFactory::makeHandle: parsing [" << r << "]" << endl;

    Request requests (ecml::RequestParser::parse(r));
    Request request (requests->value());

    Log::debug() << "LocalHandleFactory::makeHandle: request = " << request << endl;

    if (requests->rest())
        Log::warning() << "LocalHandleFactory: Only " << request << " used, skipped rest of " << requests << endl;

    ExecutionContext context;
    context.pushEnvironmentFrame(request->rest());
    if (context.getValueAsList("odbpathnameschema").size() != 1)
        throw UserError(string("\"local://\" descriptor must have one value of odbPathNameSchema: '") + r + "'");

    if (context.getValueAsList("odbserverroots").size() != 1)
        throw UserError(string("\"local://\" descriptor must have one value of odbServerRoots (string with colon separated directories): '") + r + "'");

    const string odbPathNameSchema (context.getValueAsList("odbpathnameschema")[0]);
    const string odbServerRoots (FileCollector::expandTilde(context.getValueAsList("odbserverroots")[0]));

    const vector<string> keywords (FileMapper(odbPathNameSchema).keywords());
    map<string, vector<string> > rq;

    context.pushEnvironmentFrame(request);
    for (Cell* p(request->rest()); p; p = p->rest())
    {
        const string keyword (StringTools::lower(p->text()));
        if (keyword != "server_side")
            rq[keyword] = context.getValueAsList(keyword);
        else
        {
            // This needs to be a structure more complicated than vector of strings,
            // so we need to do a trick and serialize it to a string  
            // (it's not a hack: any ECML expression can always be properly serialized).
            vector<string> ss;
            Cell* server_side (context.environment().lookup(keyword));
            ss.push_back(server_side->str());
            rq[keyword] = ss;
        }
        Log::debug() << " =+= " << keyword << " = " << rq[keyword] << endl;
    }

    MultiHandle* h (new MultiHandle);
    const string verb (StringTools::lower(request->text()));
    if (verb == "archive")
    {
        rq["source"] = context.getValueAsList("source");
        Archiver::archive(*h, keywords, rq);
    }
    else if (verb == "stage")
        Stager::stage(*h, keywords, rq);
    else
        Retriever::retrieve(*h, keywords, rq);
    return h;
}

