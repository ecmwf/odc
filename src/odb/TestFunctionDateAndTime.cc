/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionDateAndTime.cc
///
/// @author ECMWF, July 2010

//#include <iostream>
//#include <vector>
//#include <map>
//#include <algorithm>

//#define __STDC_LIMIT_MACROS
#include <cmath>

//#include <stdint.h>

//#define RMDI   -2147483647
#define EPS    1e-6

#include "eckit/utils/Timer.h"
//#include "odblib/Tool.h"
//#include "odblib/TestCase.h"
#include "odb/TestFunctionDateAndTime.h"
#include "odblib/ToolFactory.h"
//#include "odblib/piconst.h"
#include "odblib/Writer.h"
#include "odblib/ODBSelect.h"

//#include "odblib/odb_api.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionDateAndTime> _TestFunctionDateAndTime("TestFunctionDateAndTime");

TestFunctionDateAndTime::TestFunctionDateAndTime(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionDateAndTime::~TestFunctionDateAndTime() { }


void TestFunctionDateAndTime::test()
{
	testReaderIterator();
}

void TestFunctionDateAndTime::setUp()
{
	Timer t("Test DateAndTime function");
	odb::Writer<> oda("test_date_and_time.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "date", odb::INTEGER);
	row->setColumn(1, "time", odb::INTEGER);
	
	row->writeHeader();

	(*row)[0] = 20090706.0;
	(*row)[1] = 210109.0;

    ++row;
}

void TestFunctionDateAndTime::tearDown() 
{ 
	PathName("test_date_and_time.odb").unlink();
}

void TestFunctionDateAndTime::testReaderIterator()
{
    const string sql = "select julian(date,time), year(date),month(date),day(date),hour(time),minute(time),second(time), timestamp(date,time) from \"test_date_and_time.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 2455019.) < EPS); // 
	ASSERT((*it)[1] == 2009); // 
	ASSERT((*it)[2] == 7); // 
	ASSERT((*it)[3] == 6); // 
	ASSERT((*it)[4] == 21); // 
	ASSERT((*it)[5] == 1); // 
	ASSERT((*it)[6] == 9); // 
	ASSERT((*it)[7] == 20090706210109ll); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

