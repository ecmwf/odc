/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <sstream>

#include "eclib/Context.h"

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"
#include "odblib/TestRunnerApplication.h"
#include "odblib/TestRunner.h"
#include "odblib/ODBBehavior.h"

namespace odb {
namespace tool {
namespace test {

TestRunnerApplication::TestRunnerApplication (int argc, char **argv)
: ODBApplication(argc, argv)
{}

TestRunnerApplication::~TestRunnerApplication ()
{
	Log::info() << "TestRunnerApplication::~TestRunnerApplication" << endl;
}

void TestRunnerApplication::run()
{
	TestRunner testRunner(commandLineParser());
	testRunner.run();
}

} // namespace test
} // namespace tool 
} // namespace odb 

