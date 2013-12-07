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
/// @author Piotr Kuchta, ECMWF, June 2009

#include "eckit/log/Timer.h"
#include "odblib/Reader.h"

#include "UnitTest.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {



(int argc, char **argv)
: UnitTest(argc, argv)
{}

() { }

/// UnitTest DispatchingWriter
///
void UnitTest::test()
{
	const string fileName = "2000010106.odb";

	Timer t(string("UnitTest::test: reading file '") + fileName + "'");

	odb::Reader f(fileName);
	odb::Reader::iterator it = f.begin();
	odb::Reader::iterator end = f.end();

	unsigned long long n = 0;	
	for (; it != end; ++it)
		++n;

	Log::info() << "UnitTest::test decoded " << n << " lines." << std::endl;
}

void UnitTest::setUp() {}
void UnitTest::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 



