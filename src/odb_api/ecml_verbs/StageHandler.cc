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

#include "StageHandler.h"
#include "SQLHandler.h"

#include "eckit/io/MultiHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/config/Resource.h"
#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/data/DataHandleFactory.h"

#include "RetrieveHandler.h"
#include "odb_api/Stager.h"
#include "odb_api/FileMapper.h"

using namespace std;
using namespace eckit;
using namespace odb;

namespace odb {

StageHandler::StageHandler(const string& name, bool local) 
: RequestHandler(name),
  local_(local) 
{}

Values StageHandler::handle(ExecutionContext& context)
{
    Request request (Cell::clone(context.environment().currentFrame())); // TODO: delete later
    request->text("stage"); // it could be let if stage is called via apply

    Log::info() << "STAGE: " << request << endl;

    const string target (context.environment().lookup("target", "", context));
    if (! target.size()) 
        throw UserError("You must specify TARGET explicitly");

    MultiHandle input;
    if (! local_)
    {
        const string host (database(context)); 
        Log::info() << "STAGE on " << host << " save partitions info to '" << target << "'" << endl;
        DataHandleFactory::buildMultiHandle(input, string("mars://") + request->str());
    }
    else
    {
        const string odbPathNameSchema (RetrieveHandler::odbPathNameSchema(context));

        FileMapper mapper (odbPathNameSchema);
        vector<string> keywords (mapper.keywords());
        keywords.push_back("n_parts");

        map<string,vector<string> > rq;
        for (size_t i(0); i < keywords.size(); ++i)
        {
            const string& key (keywords[i]);

            rq [key] = context.getValueAsList(key);

            if (! rq[key].size() )
                throw UserError("LOCAL_STAGE: " + key + " must be set" );

            Log::info() << "STAGE: " << key << " = " << rq [ key ] << endl;
        }

        const string defaultPartitionsInfo ("partitions_info.txt");

        rq["odbPathNameSchema"].push_back(odbPathNameSchema);
        rq["odbServerRoots"].push_back(RetrieveHandler::odbServerRoots(context));
        rq["partitionsInfo"].push_back(defaultPartitionsInfo);

        Stager::stage(input, keywords, rq);
    }

    vector<PathName> r;

    FileHandle p(target);
    input.saveInto(p);
    r.push_back(target);

    ASSERT(r.size());
    List list;
    for (size_t i(0); i < r.size(); ++i)
        list.append(r[i]);
    return list;
}

} // namespace odb 

