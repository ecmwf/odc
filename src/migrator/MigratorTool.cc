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

#include "eckit/utils/StringTools.h"

#include "odblib/odb_api.h"
#include "odblib/ToolFactory.h"
#include "odblib/Tool.h"
#include "odblib/TestRunnerApplication.h"
#include "odblib/MDI.h"

#include "migrator/ODBIterator.h"
#include "migrator/FakeODBIterator.h"
#include "migrator/ReptypeGenIterator.h"
#include "migrator/ImportODBTool.h"
#include "migrator/ODB2ODATool.h"
#include "migrator/MigratorTool.h"
#include "migrator/OldODBReader.h"

namespace odb {
namespace tool {


int gdb(const vector<std::string>& params)
{
    cout << "gdb: params: " << params << std::endl;
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
int valgrind(const vector<std::string>& params)
{
    cout << "valgrind: params: " << params << std::endl;
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

void MigratorTool::run()
{
	if (parameters(1) == "g"
        || parameters(1) == "vg") {

        vector<std::string> params;
        for (size_t i(0); i < parameters().size(); ++i)
            if (i != 1)
                params.push_back(parameters()[i]);

        if (parameters(1) == "g")
            gdb(params);
        else valgrind(params);

        return;
    }
	if (parameters(1) == "test")
	{
		odb::tool::test::TestRunnerApplication(argc(), argv()).start();
		return; // TODO: Retrieve a status from the test runner
	}

    ODB2ODATool odb2oda(*this);
    odb2oda.run();
}

} // namespace tool 
} //namespace odb 

