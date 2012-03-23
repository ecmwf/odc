/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionDistance.cc
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

using namespace std;

#include "Tool.h"
#include "TestCase.h"
#include "TestFunctionDistance.h"
#include "ToolFactory.h"
#include "odblib/piconst.h"

#define SRC __FILE__, __LINE__

#include "odblib/oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionDistance> _TestFunctionDistance("TestFunctionDistance");

TestFunctionDistance::TestFunctionDistance(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionDistance::~TestFunctionDistance() { }


void TestFunctionDistance::test()
{
	testReaderIterator();
}

void TestFunctionDistance::setUp()
{
	Timer t("Test various functions to compute the distance");
	odb::Writer<> oda("test_distance.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "lat", odb::REAL);
	row->setColumn(1, "lon", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*row)[1] = 0.0;

    ++row;
}

void TestFunctionDistance::tearDown() 
{ 
	ksh("rm -f test_distance.odb", SRC);
}

void TestFunctionDistance::testReaderIterator()
{
    const string sql = "select rad(45.0,0.0,1.0,lat,lon), rad(10.0,0.0,0.0,lat,lon),distance(46.0,0.0,lat,lon),km(46.0,0.0,lat,lon),dist(100.,46.0,1.0,lat,lon), dist(40.0,5.0,1000.0,lat,lon) from \"test_distance.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

    cout << "rad(lat,lon,1.0,45.0,0.0) = " << (*it)[0] << endl;
    cout << "rad(lat,lon,0.0,10.0,0.0) = " << (*it)[1] << endl;
    cout << "distance(lat,lon,46.0,0.0) = " << (*it)[2] << endl;
    cout << "km(lat,lon,46.0,0.0) = " << (*it)[3] << endl;
    cout << "dist(lat,lon,100.,46.0,0.0) = " << (*it)[4] << endl;
    cout << "dist(lat,lon,120.,46.0,0.0) = " << (*it)[5] << endl;
	ASSERT((*it)[0] == 1); // 
	ASSERT((*it)[1] == 0); // 
	ASSERT(fabs((*it)[2] - 11112)<EPS); // 
	ASSERT(fabs((*it)[3] - 111.12e0)<EPS); // 
	ASSERT((*it)[4] == 0); // 
	ASSERT((*it)[5] == 1); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

