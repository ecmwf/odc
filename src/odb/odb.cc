/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eclib/Application.h"
#include "eclib/PathName.h"
#include "eclib/FileHandle.h"

#include "odb/TestOdaCAPI.h"

#include "odblib/oda.h"
#include "odblib/odbcapi.h"
#include "odblib/TestRunnerApplication.h"
#include "odblib/ToolFactory.h"
#include "odblib/ToolRunnerApplication.h"

using namespace std;
using namespace odb::tool;

int gdb(int argc, char *argv[]);
int valgrind(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		odb_start();
		cerr << "Usage:" << endl
			<< "        " << argv[0] << " <command> [<command's-parameters>]" << endl 
			<< "        " << argv[0] << " help <command>" << endl << endl
			<< "Available commands:" << endl;

		AbstractToolFactory::listTools(cout);
		return 1;
	}

	const string firstArg(argv[1]);

	if (firstArg == "g") { odb_start(); return gdb(argc, argv); }
	if (firstArg == "vg") { odb_start(); return valgrind(argc, argv); }
	if (firstArg == "testodacapi") return odb::tool::test::test_odacapi(argc, argv);
	if (firstArg == "test")
	{
		if (argc == 2) //no args => test all
		{
			cout << "Testing C API" << endl;

			string testCapi = string(argv[0]) + " testodacapi";
			cerr << "Executing '" << testCapi << "'" << endl;
			int rc = system(testCapi.c_str());
			if (rc) return rc;
		}

		cout << endl << "Running tests." << endl;

		odb::tool::test::TestRunnerApplication testRunner(argc - 1, argv + 1);
		testRunner.start();
		// It never really gets here.
		return 0;
	}
	if (firstArg == "help")
	{
		odb_start();
		if (argc == 2)
			AbstractToolFactory::printToolsHelp(cout);
		else
			{
			AbstractToolFactory::printToolHelp(argv[2], cout);
			cout << endl << "Usage:" << endl << endl << "\t";
			AbstractToolFactory::printToolUsage(argv[2], cout);
		}
		return 0;
	}
	if (firstArg == "-V" || firstArg == "-v" || firstArg == "--version")
	{
		cout << "ODBAPI Version: " << odb::ODBAPIVersion::version() << endl;
		cout << "File format version: " << odb::ODBAPIVersion::formatVersionMajor() << "." << odb::ODBAPIVersion::formatVersionMinor() <<  endl;
		return 0;
	}

	ToolRunnerApplication runner(argc, argv);
	runner.start();
		
	return 0;
}

int gdb(int argc, char *argv[])
{
	string cmd = argv[0];
	string args;
	string gdbScript = argv[1];
	for (int i = 2; i < argc; i++)
		args += string(" ") + argv[i];

	PathName scriptFile = string(".gdb_") + string(argv[2]);
    if (! scriptFile.exists())
	{
		string s = string("file ") + cmd + "\nbreak main\nrun " + args + "\ncatch throw\n";
		FileHandle f(scriptFile);
		f.openForWrite(1024);
		f.write(s.c_str(), s.size());
		f.close();
	}
	string vi = string("vi ") + scriptFile;
	string gdb = string("gdb -x ") + scriptFile;
	cout << "Executing '" << vi << "'" << endl;
	system(vi.c_str());
	cout << "Executing '" << gdb << "'" << endl;
	return system(gdb.c_str());
}

// valgrind --log-file=v.log --show-reachable=yes --leak-check=full ./oda test 
int valgrind(int argc, char *argv[])
{
	string cmd = argv[0];
	string args;
	for (int i = 2; i < argc; i++)
		args += string(" ") + argv[i];


	string logFile = string("vg.") + argv[2] + ".log";
	string vg = string("valgrind --log-file=") + logFile + " --show-reachable=yes --leak-check=full "
		 + " --db-attach=yes " // --suppressions=eclib.supp "
		 + cmd + " " + args;
	cout << "Executing '" << vg << "'" << endl;
	return system(vg.c_str());
}

