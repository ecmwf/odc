/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionDotp.cc
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

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestFunctionDotp.h"
#include "odblib/ToolFactory.h"
#include "odblib/piconst.h"

#include "odblib/oda.h"

using namespace std;
using namespace eckit;



namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionDotp> _TestFunctionDotp("TestFunctionDotp");

TestFunctionDotp::TestFunctionDotp(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionDotp::~TestFunctionDotp() { }


void TestFunctionDotp::test()
{
	testReaderIterator();
}

void TestFunctionDotp::setUp()
{
	Timer t("Test Dotp function");
	odb::Writer<> oda("test_dotp.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "x", odb::REAL);
	row->setColumn(1, "y", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 3.0;
	(*row)[1] = 2.0;
    ++row;
	(*row)[0] = 7.5;
	(*row)[1] = 112.0;
    ++row;
	(*row)[0] = 93.7;
	(*row)[1] = 12.3;
    ++row;
}

void TestFunctionDotp::tearDown() 
{ 
	PathName("test_dotp.odb").unlink();
}

void TestFunctionDotp::testReaderIterator()
{
    const string sql = "select dotp(x,y) from \"test_dotp.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 1998.51e0)<EPS); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

