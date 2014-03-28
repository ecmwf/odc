/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionThin.cc
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

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestFunctionThin.h"
#include "odblib/ToolFactory.h"
#include "odblib/piconst.h"

#include "odblib/odb_api.h"

using namespace std;
using namespace eclib;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionThin> _TestFunctionThin("TestFunctionThin");

TestFunctionThin::TestFunctionThin(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionThin::~TestFunctionThin() {}

void TestFunctionThin::test()
{
	testReaderIterator();
}

void TestFunctionThin::setUp()
{
	Timer t("Test thin function");
	odb::Writer<> oda("test_thin.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->setNumberOfColumns(1);

	row->setColumn(0, "lat", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
}

void TestFunctionThin::tearDown() 
{ 
	PathName("test_thin.odb").unlink();
}

void TestFunctionThin::testReaderIterator()
{
    const string sql = "select thin(2.0,lat) from \"test_thin.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

    int i=0;
	odb::Select oda(sql);
    for(odb::Select::iterator it = oda.begin(); it != oda.end(); ++it) {
      if (i % 2 == 1) 
	    ASSERT(fabs((*it)[0] - 0.0e0) < EPS); // 
      else
	    ASSERT(fabs((*it)[0] - 1.0) < EPS); // 
      ++i;
    }

}

} // namespace test 
} // namespace tool 
} // namespace odb 

