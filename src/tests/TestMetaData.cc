/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "odblib/DataStream.h"
#include "odblib/MetaData.h"

#include "UnitTest.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {



(int argc, char **argv)
: UnitTest(argc, argv)
{}



void UnitTest::setUp() {}

void UnitTest::test()
{
	typedef DataStream<SameByteOrder, DataHandle> DS;

	MetaData md1(0);
	md1.addColumn<DS>("A", "REAL");//, false, 0.);
	md1.addColumn<DS>("B", "INTEGER");//, false, 0.);

	Log::info() << "md1: " << endl << md1 << std::endl;

	MetaData md2(0);
	md2.addColumn<DS>("C", "STRING");//, false, 0.);

	Log::info() << "md2:" << endl << md2 << std::endl;

	MetaData sum = md1 + md2;
	Log::info() << "md1 + md2: " << endl <<  sum << std::endl;

	ASSERT(sum.size() == md1.size() + md2.size());
	ASSERT(sum == md1 + md2);

	MetaData sum2 = md1;
	sum2 += md2;

	ASSERT(sum2.size() == md1.size() + md2.size());
	ASSERT(sum == sum2);
}

void UnitTest::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 



