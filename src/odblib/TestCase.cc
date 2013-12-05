/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCase.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

//#include <iostream>
//#include <vector>
//#include <map>
using namespace std;

//#include "odblib/Tool.h"
//#include "odblib/TestCase.h"
//#include "odblib/ToolFactory.h"

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

