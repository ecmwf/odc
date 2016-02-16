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
#include "eckit/config/Resource.h"
#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/data/DataHandleFactory.h"

#include "odb_api/FileCollector.h"
#include "odb_api/DispatchingWriter.h"
#include "odb_api/TemplateParameters.h"

using namespace std;
using namespace eckit;
using namespace odb;

namespace odb {

RetrieveHandler::RetrieveHandler(const string& name, bool local) 
: RequestHandler(name), 
  local_(local) 
{}

std::string RetrieveHandler::odbPathNameSchema(eckit::ExecutionContext& context) 
{ 
    return FileCollector::expandTilde(valueInContextOrResource(context, "odbPathNameSchema")); 
}

std::string RetrieveHandler::odbServerRoots(eckit::ExecutionContext& context) 
{ 
    return FileCollector::expandTilde(valueInContextOrResource(context, "odbServerRoots")); 
} 

std::string RetrieveHandler::valueInContextOrResource(eckit::ExecutionContext& context, const string& keyword, bool required)
{
    string r (context.environment().lookup(keyword, eckit::Resource<std::string>(keyword, ""), context));
    if (required && ! r.size())
        throw UserError(string("Value of ") + keyword + " not found in neither ECML context nor config resource.");
    return r;
}

Values RetrieveHandler::handle(ExecutionContext& context)
{
    Request request (Cell::clone(context.environment().currentFrame())); // TODO: delete later
    request->text("retrieve"); // TODO: we should not need to do this

    Log::info() << "RetrieveHandler::handle: request: " << request << endl;
 
    const string host (database(context)),
                 target (context.environment().lookup("target", "", context)),
                 filter (context.environment().lookup("filter", "", context));

    if (! target.size())
        throw UserError("You must specify TARGET explicitly");

    Log::info() << "RETRIEVE from " << host << " into " << target << endl;

    stringstream ss;
    ss << (local_ ? "local://" : "mars://") << request;
    if (local_)
        ss << ",odbPathNameSchema=\"" << odbPathNameSchema(context) << "\""
           << ",odbServerRoots=\"" << odbServerRoots(context) << "\"";

    MultiHandle input;
    DataHandleFactory::buildMultiHandle(input, ss.str());
    Log::info() << "RETRIEVE input " << input << endl;

    TemplateParameters templateParameters;
    TemplateParameters::parse(target, templateParameters);

    vector<PathName> r;
    if (filter.size() || templateParameters.size())
    {
        try { 
            r = SQLHandler::executeSelect(filter, input, target, &context);
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
    List list;
    for (size_t i(0); i < r.size(); ++i)
        list.append(r[i]);
    return list;
}

} // namespace odb 

