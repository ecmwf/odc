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
#include "odc/Select.h"

#include "odc/Writer.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;


static void setUp()
{
	Timer t("Test rgg_boxlat and rgg_boxlon functions");
	odc::Writer<> oda("test_rgg_box.odb");

	odc::Writer<>::iterator row = oda.begin();
	row->setNumberOfColumns(2);

	row->setColumn(0, "lat", odc::REAL);
	row->setColumn(1, "lon", odc::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*row)[1] = 5.0;

    ++row;
}

static void tearDown() 
{ 
	PathName("test_rgg_box.odb").unlink();
}

static void test()
{
	ASSERT(getenv("ODB_RTABLE_PATH") && "environment variable must be set for rgg_boxlat and rgg_boxlon to work properly");
    const string sql = "select rgg_boxlat(lat,lon,31), rgg_boxlon(lat,lon,31) from \"test_rgg_box.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odc::Select oda(sql);
	odc::Select::iterator it = oda.begin();

	Log::info() << "UnitTest: '" << sql << "' => [" << (*it)[0] << ", " << (*it)[1] << "]" << std::endl;

	ASSERT(fabs((*it)[0] - 47.069642059688e0)<EPS); // 
	ASSERT(fabs((*it)[1] - 6)<EPS); // 

}



SIMPLE_TEST(FunctionRggBox)
