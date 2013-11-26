/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCommandLineParsing.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#include <iostream>
#include <vector>
#include <map>

#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odb/TestCommandLineParsing.h"
#include "odblib/ToolFactory.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestCommandLineParsing> testCommandLineParsing("TestCommandLineParsing");

struct TestTool : public Tool {

    TestTool(int argc, char **argv) : Tool(argc, argv)
	{
		registerOptionWithArgument("-foo");
		registerOptionWithArgument("-intOpt");
	}

	static void help(std::ostream &o) { o << "No help available for this command yet." << std::endl; }

	void run() {
		Log::info() << "TestCommandLineParsing::test: TestTool::run" << std::endl;

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

