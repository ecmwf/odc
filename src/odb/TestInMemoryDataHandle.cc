/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestInMemoryDataHandle.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/TestCase.h"
#include "odblib/InMemoryDataHandle.h"
#include "TestInMemoryDataHandle.h"

using namespace std;
using namespace eckit;

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

