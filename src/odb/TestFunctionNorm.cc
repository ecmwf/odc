/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionNorm.cc
///
/// @author ECMWF, July 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#define RMDI   -2147483647
#define EPS    4e-5

using namespace std;

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestFunctionNorm.h"
#include "odblib/ToolFactory.h"
#include "odblib/piconst.h"

#include "odblib/odb_api.h"

using namespace std;
using namespace eclib;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionNorm> _TestFunctionNorm("TestFunctionNorm");

TestFunctionNorm::TestFunctionNorm(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionNorm::~TestFunctionNorm() { }


void TestFunctionNorm::test()
{
	testReaderIterator();
}

void TestFunctionNorm::setUp()
{
	Timer t("Test Norm function");
	odb::Writer<> oda("test_norm.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "x", odb::REAL);
	row->setColumn(1, "y", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 3.0;
	(*row)[1] = 16.0;
    ++row;
	(*row)[0] = 4.0;
	(*row)[1] = 12.0;
    ++row;
	(*row)[0] = 2.0;
	(*row)[1] = 24.0;
    ++row;
}

void TestFunctionNorm::tearDown() 
{ 
	PathName("test_norm.odb").unlink();
}

void TestFunctionNorm::testReaderIterator()
{
    const string sql = "select norm(x,y) from \"test_norm.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 12e0)<EPS); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

