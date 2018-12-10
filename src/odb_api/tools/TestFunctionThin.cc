/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.cc
///
/// @author ECMWF, July 2010

const double EPS =   7e-6;

#include <cmath>

#include "eckit/log/Timer.h"
#include "odb_api/Select.h"

#include "odb_api/Writer.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;


static void setUp()
{
	Timer t("Test thin function");
	odc::Writer<> oda("test_thin.odb");

	odc::Writer<>::iterator row = oda.begin();
	row->setNumberOfColumns(1);

	row->setColumn(0, "lat", odc::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
}

static void tearDown() 
{ 
	PathName("test_thin.odb").unlink();
}

static void test()
{
    const string sql = "select thin(2.0,lat) from \"test_thin.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

    int i=0;
	odc::Select oda(sql);
    for(odc::Select::iterator it = oda.begin(); it != oda.end(); ++it) {
      if (i % 2 == 1) 
	    ASSERT(fabs((*it)[0] - 0.0e0) < EPS); // 
      else
	    ASSERT(fabs((*it)[0] - 1.0) < EPS); // 
      ++i;
    }

}


SIMPLE_TEST(FunctionThin)
