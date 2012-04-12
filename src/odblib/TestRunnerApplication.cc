/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <sstream>

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"
#include "odblib/TestRunnerApplication.h"
#include "odblib/TestRunner.h"

namespace odb {
namespace tool {
namespace test {

TestRunnerApplication::TestRunnerApplication (int argc, char **argv)
: Application(argc, argv),
  argc_(argc),
  argv_(argv)
{}

TestRunnerApplication::~TestRunnerApplication ()
{
	Log::info() << "TestRunnerApplication::~TestRunnerApplication" << endl;
}

void TestRunnerApplication::run()
{
	TestRunner testRunner(argc_, argv_);
	testRunner.run();
}

} // namespace test
} // namespace tool 
} // namespace odb 

