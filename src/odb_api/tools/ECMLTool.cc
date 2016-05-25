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

void ECMLTool::executeRC(ExecutionContext& context)
{
    const PathName ecmlrc (string(getenv("HOME")) + "/.ecmlrc");
    if (ecmlrc.exists())
        try { 
            Log::info() << "Executing " << ecmlrc << endl;
            context.executeScriptFile(ecmlrc);
        } catch (std::exception e)
        {
            Log::info() << "Exception while trying to execute " << ecmlrc << ":" << e.what() << endl;
        }
}

void ECMLTool::run()
{
    ExecutionContext context;
    ODBModule odbModule;
    context.import(odbModule);

    executeRC(context);

    if (parameters().size() < 2)
    {
        REPLHandler::repl(context);
        return;
    }

    for (size_t i (1); i < argc(); ++i)
    {
        const string param (argv()[i]);
        if (param == "-e") 
        {
            const string& e (argv()[++i]);

            Log::info() << "Trying to execute expression '" << e << "':" << endl;
            context.execute(e);
        }
        else
            context.executeScriptFile(param);
    }
}

} // namespace tool 
} // namespace odb 

