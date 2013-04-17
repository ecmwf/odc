/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionsForAngleConversion.cc
///
/// @author ECMWF, July 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#define RMDI   -2147483647
#define EPS    7e-6

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestFunctionsForAngleConversion.h"
#include "odblib/ToolFactory.h"
#include "odblib/piconst.h"

#include "odblib/odb_api.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionsForAngleConversion> _TestFunctionsForAngleConversion("TestFunctionsForAngleConversion");

TestFunctionsForAngleConversion::TestFunctionsForAngleConversion(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionsForAngleConversion::~TestFunctionsForAngleConversion() { }


void TestFunctionsForAngleConversion::test()
{
	testReaderIterator();
}

void TestFunctionsForAngleConversion::setUp()
{
	Timer t("Test various functions to convert angles (radians to degrees, etc.)");
	odb::Writer<> oda("test_angleconv.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "radian_col", odb::REAL);
	row->setColumn(1, "degrees_col", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = piconst::pi;
	(*row)[1] = 180.0e0;
	++row;

	(*row)[0] = 0.0e0;
	(*row)[1] = 0.0e0;
	++row;

	(*row)[0] = piconst::pi/4.0e0;
	(*row)[1] = 45.0e0;
	++row;
}

void TestFunctionsForAngleConversion::tearDown() 
{ 
	PathName("test_angleconv.odb").unlink();
}

void TestFunctionsForAngleConversion::testReaderIterator()
{
    const string sql = "select degrees(radian_col),radians(degrees_col), rad2deg(radian_col), deg2rad(degrees_col), radians(degrees(radian_col)), degrees(radians(degrees_col)) from \"test_angleconv.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

// because stored as single real precision; we loose some accuracy
	ASSERT(fabs((*it)[0] - 180) < EPS); // 
	ASSERT(fabs((*it)[1] - piconst::pi) < EPS); //
	ASSERT(fabs((*it)[2] - 180.0) < EPS); // 
	ASSERT(fabs((*it)[3] - piconst::pi) < EPS);    // 

	ASSERT(fabs((*it)[4] - piconst::pi) < EPS); // 
	ASSERT(fabs((*it)[5] - 180.0e0) < EPS);    //

    ++it;
    ASSERT(fabs((*it)[0]) < EPS); // 
    ASSERT(fabs((*it)[1]) < EPS); //
    ASSERT(fabs((*it)[2]) < EPS); // 
    ASSERT(fabs((*it)[3]) < EPS);    // 

    ASSERT(fabs((*it)[4]) < EPS); // 
    ASSERT(fabs((*it)[5]) < EPS);    //

    ++it;
    ASSERT(fabs((*it)[0] - 45) < EPS); // 
    ASSERT(fabs((*it)[1] - piconst::pi/4.0) < EPS); //
    ASSERT(fabs((*it)[2] - 45.0) < EPS); // 
    ASSERT(fabs((*it)[3] - piconst::pi/4.0) < EPS);    // 

    ASSERT(fabs((*it)[4] - piconst::pi/4.0) < EPS); // 
    ASSERT(fabs((*it)[5] - 45.0e0) < EPS);    //


}

} // namespace test 
} // namespace tool 
} // namespace odb 

