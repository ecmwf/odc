/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionRggBox.cc
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

using namespace std;

#include "Tool.h"
#include "TestCase.h"
#include "TestFunctionRggBox.h"
#include "ToolFactory.h"
#include "odblib/piconst.h"

#define SRC __FILE__, __LINE__

#include "odblib/oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionRggBox> _TestFunctionRggBox("TestFunctionRggBox");

TestFunctionRggBox::TestFunctionRggBox(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionRggBox::~TestFunctionRggBox() { }


void TestFunctionRggBox::test()
{
	testReaderIterator();
}

void TestFunctionRggBox::setUp()
{
	Timer t("Test rgg_boxlat and rgg_boxlon functions");
	odb::Writer<> oda("test_rgg_box.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "lat", odb::REAL);
	row->setColumn(1, "lon", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*row)[1] = 5.0;

    ++row;
}

void TestFunctionRggBox::tearDown() 
{ 
	ksh("rm -f test_rgg_box.odb", SRC);
}

void TestFunctionRggBox::testReaderIterator()
{
    const string sql = "select rgg_boxlat(lat,lon,31), rgg_boxlon(lat,lon,31) from \"test_rgg_box.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 47.069642059688e0)<EPS); // 
	ASSERT(fabs((*it)[1] - 6)<EPS); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

