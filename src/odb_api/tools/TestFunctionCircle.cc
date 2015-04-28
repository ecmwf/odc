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
using namespace odb;


static void setUp()
{
	Timer t("Test Circle function");
	odb::Writer oda("test_circle.odb");

	odb::Writer::iterator row = oda.begin();
	row->setNumberOfColumns(2);

	row->setColumn(0, "lat", odb::REAL);
	row->setColumn(1, "lon", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*row)[1] = 10.0;

    ++row;
}

static void tearDown() 
{ 
	PathName("test_circle.odb").unlink();
}

static void test()
{
    const string sql = "select circle(lat,46.0, lon,11.0,1.0), circle(lat,46.0, lon,11.0,1.5) from \"test_circle.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 0.0e0) < EPS); // 
	ASSERT(fabs((*it)[1] - 1.0) < EPS); // 

}



SIMPLE_TEST(FunctionCircle)
