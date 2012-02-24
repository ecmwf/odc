/// \file TestTEMPLATE.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

using namespace std;

#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "TestCase.h"

#include "TestTEMPLATE.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestTEMPLATE> _TestTEMPLATE("TestTEMPLATE");

TestTEMPLATE::TestTEMPLATE(int argc, char **argv)
: TestCase(argc, argv)
{}

TestTEMPLATE::~TestTEMPLATE() { }

void TestTEMPLATE::setUp() {}


void TestTEMPLATE::test()
{
	Log::info() << "I'm just a template, I don't test anything, really." << endl;
}

void TestTEMPLATE::tearDown() { }

} // namespace test 
} // namespace tool 
} // namespace odb 

