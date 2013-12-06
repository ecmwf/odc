/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionCircle.cc
///
/// @author ECMWF, July 2010

#include <cmath>
//#include <iostream>
//#include <vector>
//#include <map>
//#include <algorithm>

#define __STDC_LIMIT_MACROS

//#include <stdint.h>

//#define RMDI -2147483647
#define EPS  7e-6

//#include "odblib/Tool.h"
//#include "odblib/TestCase.h"
#include "odb/TestFunctionCircle.h"
#include "odblib/ToolFactory.h"
//#include "odblib/piconst.h"
#include "odblib/Writer.h"
#include "odblib/ODBSelect.h"

//#include "odblib/odb_api.h"
#include "eckit/utils/Timer.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionCircle> _TestFunctionCircle("TestFunctionCircle");

TestFunctionCircle::TestFunctionCircle(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionCircle::~TestFunctionCircle() { }


void TestFunctionCircle::test()
{
	testReaderIterator();
}

void TestFunctionCircle::setUp()
{
	Timer t("Test Circle function");
	odb::Writer<> oda("test_circle.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "lat", odb::REAL);
	row->setColumn(1, "lon", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*row)[1] = 10.0;

    ++row;
}

void TestFunctionCircle::tearDown() 
{ 
	PathName("test_circle.odb").unlink();
}

void TestFunctionCircle::testReaderIterator()
{
    const string sql = "select circle(lat,46.0, lon,11.0,1.0), circle(lat,46.0, lon,11.0,1.5) from \"test_circle.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 0.0e0) < EPS); // 
	ASSERT(fabs((*it)[1] - 1.0) < EPS); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

