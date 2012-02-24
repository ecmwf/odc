/// \file TestCommandLineParsing.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "Log.h"
#include "Exceptions.h"

#include "Tool.h"
#include "TestCase.h"
#include "TestCommandLineParsing.h"
#include "ToolFactory.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestCommandLineParsing> testCommandLineParsing("TestCommandLineParsing");

struct TestTool : public Tool {
	TestTool(int argc, char **argv) : Tool(argc, argv)
	{
		registerOptionWithArgument("-foo");
		registerOptionWithArgument("-intOpt");
	};

	static void help(ostream &o) { o << "No help available for this command yet." << endl; };

	void run() {
		Log::info() << "TestCommandLineParsing::test: TestTool::run" << endl;

		ASSERT(optionArgument("-foo", std::string("NONE")) == "bar");
		ASSERT(optionArgument("-intOpt", 0) == 69);

		ASSERT(optionIsSet("-blah"));
		ASSERT(optionIsSet("-blahblah"));
		ASSERT(! optionIsSet("-blahblahblah"));
		ASSERT(optionIsSet("-lastOption"));

		ASSERT(parameters().size() == 3);
		ASSERT(parameters()[0] == "p1");
		ASSERT(parameters()[1] == "p2");
		ASSERT(parameters()[2] == "p3");
		
	}
};

TestCommandLineParsing::TestCommandLineParsing(int argc, char **argv)
: TestCase(argc, argv)
{}

void TestCommandLineParsing::test()
{
	const char *args[] = {"-foo", "bar", "-intOpt", "69", "-blah", "-blahblah", "p1", "p2", "-lastOption", "p3", 0};

	TestTool testTool(sizeof(args) / sizeof(char *) - 1, const_cast<char **>(args));
	testTool.run();
}

} // namespace test 
} // namespace tool 
} // namespace odb 

