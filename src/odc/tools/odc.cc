/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/FileHandle.h"

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "odc/ODBAPIVersion.h"
#include "odc/odccapi.h"

#include "TestOdaCAPI.h"
#include "TestRunnerApplication.h"
#include "Tool.h"
#include "ToolFactory.h"
#include "ToolRunnerApplication.h"

using namespace std;
using namespace eckit;

using namespace odc::tool;

int executeCommand(int argc, char* argv[]);
int gdb(int argc, char* argv[]);
int valgrind(int argc, char* argv[]);
int sqlhelp(int argc, char* argv[]);

// This is a hack to ensure that the odctest library links in properly
#include "odc/tools/TestOdaCAPI.h"
int (*global_odctest_hack)(int, char**) = &::odc::tool::test::test_odacapi_setup;

int main(int argc, char* argv[]) {
    try {
        return executeCommand(argc, argv);
    }
    catch (std::exception& e) {
        cerr << argv[0] << ": " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        cerr << argv[0] << ": unknown exception" << std::endl;
        return 1;
    }
}

int executeCommand(int argc, char* argv[]) {
    odc::tool::Tool::registerTools();
    if (argc < 2) {
        odb_start_with_args(argc, argv);
        cerr << "Usage:" << endl
             << "        " << argv[0] << " <command> [<command's-parameters>]" << endl
             << "        " << argv[0] << " help <command>" << std::endl
             << endl
             << "Available commands:" << std::endl;

        AbstractToolFactory::listTools(cout);
        return 1;
    }

    const string firstArg(argv[1]);

    if (firstArg == "g") {
        odb_start_with_args(argc, argv);
        return gdb(argc, argv);
    }
    if (firstArg == "vg") {
        odb_start_with_args(argc, argv);
        return valgrind(argc, argv);
    }

    /// TODO: reenable
    //	if (firstArg == "testodbcapi") return odc::tool::test::test_odacapi(argc, argv);
    if (firstArg == "test") {
        if (argc == 2)  // no args => test all
        {
            std::cout << "Testing C API" << std::endl;

            string testCapi = std::string(argv[0]) + " testodbcapi";
            cerr << "Executing '" << testCapi << "'" << std::endl;
            int rc = system(testCapi.c_str());
            if (rc)
                return rc;
        }

        std::cout << std::endl << "Running tests." << std::endl;

        odc::tool::test::TestRunnerApplication testRunner(argc - 1, argv + 1);
        testRunner.start();
        // It never really gets here.
        return 0;
    }

    if (firstArg == "help") {
        odb_start_with_args(argc, argv);
        if (argc == 2)
            AbstractToolFactory::printToolsHelp(cout);
        else {
            AbstractToolFactory::printToolHelp(argv[2], cout);
            std::cout << std::endl << "Usage:" << std::endl << std::endl << "\t";
            AbstractToolFactory::printToolUsage(argv[2], cout);
        }
        return 0;
    }

    if (firstArg == "sqlhelp")
        return sqlhelp(argc, argv);

    if (firstArg == "-V" || firstArg == "-v" || firstArg == "--version") {
        std::cout << "ODBAPI Version: " << odc::ODBAPIVersion::version() << std::endl;
        std::cout << "File format version: " << odc::ODBAPIVersion::formatVersionMajor() << "."
                  << odc::ODBAPIVersion::formatVersionMinor() << std::endl;
        return 0;
    }

    ToolRunnerApplication runner(argc, argv);
    return runner.start();
}

int gdb(int argc, char* argv[]) {
    string cmd = argv[0];
    string args;
    string gdbScript = argv[1];
    for (int i = 2; i < argc; i++)
        args += std::string(" ") + argv[i];

    PathName scriptFile(std::string(".gdb_") + std::string(argc < 3 ? "odc" : argv[2]));

    if (!scriptFile.exists()) {
        string s = std::string("file ") + cmd + "\nbreak main\nrun " + args + "\ncatch throw\n";
        FileHandle f(scriptFile);
        f.openForWrite(1024);
        f.write(s.c_str(), s.size());
        f.close();
    }
    string vi  = std::string("vi ") + scriptFile;
    string gdb = std::string("gdb -x ") + scriptFile;
    std::cout << "Executing '" << vi << "'" << std::endl;
    system(vi.c_str());
    std::cout << "Executing '" << gdb << "'" << std::endl;
    return system(gdb.c_str());
}

// valgrind --log-file=v.log --show-reachable=yes --leak-check=full ./oda test
int valgrind(int argc, char* argv[]) {
    string cmd = argv[0];
    string args;
    for (int i = 2; i < argc; i++)
        args += std::string(" ") + argv[i];


    string logFile = std::string("vg.") + argv[2] + ".log";
    string vg      = std::string("valgrind --log-file=") + logFile + " --show-reachable=yes --leak-check=full " +
                " --db-attach=yes "  // --suppressions=eckit.supp "
                + cmd + " " + args;
    std::cout << "Executing '" << vg << "'" << std::endl;
    return system(vg.c_str());
}

int sqlhelp(int argc, char* argv[]) {
    auto info = eckit::sql::expression::function::FunctionFactory::instance().functionsInfo();

    for (auto& i : info) {
        std::cout << i.name << "/" << i.arity << " " << i.help << std::endl;
    }

    return 0;
}
