/// \file TestDecoding.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include <iostream>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestDecoding.h"
#include "ToolFactory.h"
#include "SimpleFilterIterator.h"
#include "SimpleFilter.h"
#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestDecoding> _TestDecoding("TestDecoding");

TestDecoding::TestDecoding(int argc, char **argv)
: TestCase(argc, argv)
{}

TestDecoding::~TestDecoding() { }

/// Tests DispatchingWriter
///
void TestDecoding::test()
{
	const string fileName = "../odb2oda/2000010106/ECMA.odb";

	Timer t(string("TestDecoding::test: reading file '") + fileName + "'");

	odb::Reader f(fileName);
	odb::Reader::iterator it = f.begin();
	odb::Reader::iterator end = f.end();

	unsigned long long n = 0;	
	for (; it != end; ++it)
		++n;

	Log::info() << "TestDecoding::test decoded " << n << " lines." << endl;
}

void TestDecoding::setUp() {}
void TestDecoding::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

