/// \file TestInMemoryDataHandle.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>
using namespace std;

#include "Tool.h"
#include "ToolFactory.h"
#include "TestCase.h"
#include "InMemoryDataHandle.h"
#include "TestInMemoryDataHandle.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestInMemoryDataHandle> _TestInMemoryDataHandle("TestInMemoryDataHandle");

TestInMemoryDataHandle::TestInMemoryDataHandle(int argc, char **argv)
: TestCase(argc, argv)
{}

TestInMemoryDataHandle::~TestInMemoryDataHandle() {}

void TestInMemoryDataHandle::setUp() {}
void TestInMemoryDataHandle::test()
{
	char data[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
	char data2[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
	bzero(data2, sizeof(data2));

	h.write(data, sizeof(data));
	Length len = h.openForRead();
	Log::info() << "Len = " << len << endl;
	ASSERT(len == Length(sizeof(data)));
	h.read(data2, sizeof(data2));

	for (size_t i = 0; i < sizeof(data); i++)
	{
		Log::info() << "data[i]=" << (int) data[i] << ", data2[i]=" << (int) data2[i] << endl;
		ASSERT(data[i] == data2[i]);
	}
}

void TestInMemoryDataHandle::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

