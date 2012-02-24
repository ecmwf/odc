/// \file TestMetaData.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

using namespace std;

#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "TestCase.h"

#include "MetaData.h"
#include "DataStream.h"
#include "TestMetaData.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestMetaData> _TestMetaData("TestMetaData");

TestMetaData::TestMetaData(int argc, char **argv)
: TestCase(argc, argv)
{}

TestMetaData::~TestMetaData() {}

void TestMetaData::setUp() {}

void TestMetaData::test()
{
	typedef DataStream<SameByteOrder, DataHandle> DS;

	MetaData md1(0);
	md1.addColumn<DS>("A", "REAL", false, 0.);
	md1.addColumn<DS>("B", "INTEGER", false, 0.);

	Log::info() << "md1: " << endl << md1 << endl;

	MetaData md2(0);
	md2.addColumn<DS>("C", "STRING", false, 0.);

	Log::info() << "md2:" << endl << md2 << endl;

	MetaData sum = md1 + md2;
	Log::info() << "md1 + md2: " << endl <<  sum << endl;

	ASSERT(sum.size() == md1.size() + md2.size());
	ASSERT(sum == md1 + md2);

	MetaData sum2 = md1;
	sum2 += md2;

	ASSERT(sum2.size() == md1.size() + md2.size());
	ASSERT(sum == sum2);
}

void TestMetaData::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

