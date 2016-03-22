/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/prelude/REPLHandler.h"

#include "odb_api/odb_api.h"
#include "odb_api/ODBModule.h"
#include "odb_api/tools/ECMLTool.h"

using namespace std;
using namespace eckit;
using namespace odb::sql;

namespace odb {
namespace tool {

ECMLTool::ECMLTool(int argc, char **argv) : Tool(argc, argv) {}

ECMLTool::~ECMLTool() {}

void ECMLTool::run()
{
    ExecutionContext context;
    ODBModule odbModule;
    context.import(odbModule);

    const PathName ecmlrc (string(getenv("HOME")) + "/.ecmlrc");
    if (ecmlrc.exists())
        try { 
            Log::info() << "Executing " << ecmlrc << endl;
            context.executeScriptFile(ecmlrc);
        } catch (std::exception e)
        {
            Log::info() << "Exception while trying to execute " << ecmlrc << ":" << e.what() << endl;
        }

    if (parameters().size() < 2)
    {
        //Log::error() << "Usage: ";
        //usage(parameters(0), Log::error());
        //Log::error() << std::endl;
        REPLHandler::repl(context);
        return;// 1;
    }

    std::vector<std::string> params(parameters());
    params.erase(params.begin());
    for (size_t i (0); i < params.size(); ++i)
    {
        context.executeScriptFile(params[i]);
    }
}

} // namespace tool 
} // namespace odb 

