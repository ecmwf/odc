/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileHandle.h"
#include "odb_api/migrator/FakeODBIterator.h"
#include "odb_api/migrator/MigratorTool.h"
#include "odb_api/migrator/ODB2ODATool.h"
#include "odb_api/migrator/ODBIterator.h"
#include "odb_api/migrator/ReptypeGenIterator.h"
#include "odb_api/odb_api.h"
#include "odb_api/tools/Tool.h"
#include "odb_api/tools/ToolFactory.h"
#include "odb_api/tools/ToolRunnerApplication.h"


using namespace std;
using namespace odb::tool;

// Cannot use just string for str because of a clash with a typedef in ODB header...
typedef std::string str;

int gdb(int argc, char *argv[]);
int valgrind(int argc, char *argv[]);

//void test_schemaFile();

int main(int argc, char *argv[])
{
    CommandLineParser clp(argc, argv);
	clp.registerOptionWithArgument("-genreptype");
    clp.registerOptionWithArgument("-reptypecfg");
    clp.registerOptionWithArgument("-addcolumns");
    clp.registerOptionWithArgument("-mdi");
    clp.parameters();

    cout << clp << std::endl;

	ToolRunnerApplication runner(argc, argv, false, false);
	//ToolRunnerApplication runner(clp, false, false);
	MigratorTool migrator(clp);
	runner.tool(&migrator);
	return runner.start();
}

#if 0
#include <assert.h>

void test_schemaFile() {
	assert(ODBIterator::schemaFile("/asdfasd/sdfas/ECMA.tmi") == "/asdfasd/sdfas/ECMA.tmi/ECMA.sch");
	assert(ODBIterator::schemaFile("/asdfasd/sdfas/ODA.tmi") == "/asdfasd/sdfas/ODA.tmi/ODA.sch");
	assert(ODBIterator::schemaFile("/asdfasd/sdfas/ODA.tmi/") == "/asdfasd/sdfas/ODA.tmi/ODA.sch");
	assert(ODBIterator::schemaFile("ODA.tmi/") == "ODA.tmi/ODA.sch");
	assert(ODBIterator::schemaFile("ODA.tmi") == "ODA.tmi/ODA.sch");
	assert(ODBIterator::schemaFile("dupa/ODA.tmi") == "dupa/ODA.tmi/ODA.sch");
}
#endif 


