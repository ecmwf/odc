/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionEqBox.cc
///
/// @author ECMWF, July 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#define RMDI -2147483647
#define EPS  7e-6

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestFunctionEqBox.h"
#include "odblib/ToolFactory.h"
#include "odblib/piconst.h"

#include "odblib/oda.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionEqBox> _TestFunctionEqBox("TestFunctionEqBox");

TestFunctionEqBox::TestFunctionEqBox(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionEqBox::~TestFunctionEqBox() { }


void TestFunctionEqBox::test()
{
	testReaderIterator();
}

void TestFunctionEqBox::setUp()
{
	Timer t("Test eq_boxlat and eq_boxlon functions");
	odb::Writer<> oda("test_eq_box.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "lat", odb::REAL);
	row->setColumn(1, "lon", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*row)[1] = 5.0;

    ++row;
}

void TestFunctionEqBox::tearDown() 
{ 
	PathName("test_eq_box.odb").unlink();
}

void TestFunctionEqBox::testReaderIterator()
{
    const string sql = "select eq_boxlat(lat,lon,10.5), eq_boxlon(lat,lon,10.5) from \"test_eq_box.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 47.134066345052e0)<EPS); // 
	ASSERT((*it)[1] == 0); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

