/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <Application.h>
#include <PathName.h>
#include <FileHandle.h>
#include <Timer.h>
#include <Tokenizer.h>

#include <sstream>

#include "Tool.h"
#include "TestCase.h"
#include "ToolFactory.h"
#include "TestRunnerApplication.h"
#include "TestRunner.h"

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

