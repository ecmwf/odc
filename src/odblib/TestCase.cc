/// \file TestCase.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"

namespace odb {
namespace tool {
namespace test {

TestCase::TestCase(int argc, char **argv)
: Tool(argc, argv)
{}

void TestCase::run()
{
	setUp();
	test();
	tearDown();
}

TestCase::~TestCase() {}

void TestCase::setUp() {}
void TestCase::test() {}
void TestCase::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

