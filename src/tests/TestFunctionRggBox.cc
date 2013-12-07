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
#include "odblib/Select.h"

#include "odblib/Writer.h"
#include "UnitTest.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {



void UnitTest::test()
{
	testReaderIterator();
}

void UnitTest::setUp()
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

void UnitTest::tearDown() 
{ 
	PathName("test_rgg_box.odb").unlink();
}

void UnitTest::testReaderIterator()
{
	ASSERT(getenv("ODB_RTABLE_PATH") && "environment variable must be set for rgg_boxlat and rgg_boxlon to work properly");
    const string sql = "select rgg_boxlat(lat,lon,31), rgg_boxlon(lat,lon,31) from \"test_rgg_box.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	Log::info() << "UnitTest: '" << sql << "' => [" << (*it)[0] << ", " << (*it)[1] << "]" << std::endl;

	ASSERT(fabs((*it)[0] - 47.069642059688e0)<EPS); // 
	ASSERT(fabs((*it)[1] - 6)<EPS); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 



