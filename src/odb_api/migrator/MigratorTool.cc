/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file MigratorTool.cc
///
/// @author Piotr Kuchta, ECMWF, July 2009
///

#include <unistd.h>
#include <iostream>
#include <fstream>

#include "eckit/parser//StringTools.h"
#include "eckit/ecml/core/ExecutionContext.h"

#include "odb_api/migrator/FakeODBIterator.h"
#include "odb_api/migrator/ImportODBTool.h"
#include "odb_api/migrator/MigratorTool.h"
#include "odb_api/migrator/ODB2ODATool.h"
#include "odb_api/migrator/ODBIterator.h"
#include "odb_api/migrator/OldODBReader.h"
#include "odb_api/migrator/ReptypeGenIterator.h"
#include "odb_api/MDI.h"
#include "odb_api/odb_api.h"
#include "odb_api/tools/Tool.h"
#include "odb_api/tools/ToolFactory.h"
#include "odb_api/ODBModule.h"

#include "ODBMigratorModule.h"

using namespace eckit;
using namespace std;

namespace odb {
namespace tool {


int gdb(const std::vector<std::string>& params)
{
    std::cout << "gdb: params: " << params << std::endl;
	str cmd(params[0]);
	str args;
	for (size_t i = 1; i < params.size(); ++i)
		args += str(" ") + params[i];

    eckit::PathName scriptFile = str(".gdb_") + params[2];
	if (! scriptFile.exists())
	{
		str s = str("file ") + cmd + "\nbreak main\nrun " + args + "\n";
        s += "catch throw\n";
        eckit::FileHandle f(scriptFile);
		f.openForWrite(1024);
		f.write(s.c_str(), s.size());
		f.close();
	}
	str vi = str("vi ") + scriptFile;
	std::cout << "Executing '" << vi << "'" << std::endl;
	system(vi.c_str());

	str gdbCmd = str("gdb -x ") + scriptFile;
	std::cout << "Executing '" << gdbCmd << "'" << std::endl;
	return system(gdbCmd.c_str());
}

// valgrind --log-file=v.log --show-reachable=yes --leak-check=full ./oda test 
int valgrind(const std::vector<std::string>& params)
{
    std::cout << "valgrind: params: " << params << std::endl;
	str cmd(params[0]);
	str args;
	for (size_t i = 1; i < params.size(); ++i)
		args += str(" ") + params[i];

	str logFile = str("vg.log");
	str vg = str("valgrind --log-file=") + logFile + " --show-reachable=yes --leak-check=full " + cmd + " " + args;
	std::cout << "Executing '" << vg << "'" << std::endl;
	return system(vg.c_str());
}

//MigratorTool::MigratorTool (int argc, char *argv[]) : Tool(argc, argv) { } 

MigratorTool::MigratorTool (const CommandLineParser &clp) : Tool(clp) { } 

void MigratorTool::runECML()
{
    ExecutionContext context;
    ODBModule odbModule;
    ODBMigratorModule migratorModule;
    context.import(odbModule);
    context.import(migratorModule);

    std::vector<std::string> params(parameters());
    params.erase(params.begin());
    params.erase(params.begin());
    for (size_t i (0); i < params.size(); ++i)
    {
        Log::info() << "*** Executing " << params[i] << endl;
        context.executeScriptFile(params[i]);
    }

}

void MigratorTool::run()
{
    if (parameters().size() > 1)
    {
        if (parameters(1) == "g" || parameters(1) == "vg")
        {
            std::vector<std::string> params;
            for (size_t i(0); i < parameters().size(); ++i)
                if (i != 1)
                    params.push_back(parameters()[i]);

            if (parameters(1) == "g")
                gdb(params);
            else valgrind(params);

            return;
        }
        if (parameters(1) == "ecml")
            return runECML();

        if (parameters(1) == "test")
        {
            //odb::tool::test::TestRunnerApplication(argc(), argv()).start();
            return; // TODO: Retrieve a status from the test runner
        }
    }

    ODB2ODATool odb2oda(*this);
    odb2oda.run();
}

} // namespace tool 
} //namespace odb 

