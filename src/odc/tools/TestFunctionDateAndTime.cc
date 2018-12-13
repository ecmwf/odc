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

const double EPS =     1e-6;

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
	Timer t("Test DateAndTime function");
	odc::Writer<> oda("test_date_and_time.odb");

	odc::Writer<>::iterator row = oda.begin();
	row->setNumberOfColumns(2);

	row->setColumn(0, "date", odc::INTEGER);
	row->setColumn(1, "time", odc::INTEGER);
	
	row->writeHeader();

	(*row)[0] = 20090706.0;
	(*row)[1] = 210109.0;

    ++row;
}

static void tearDown() 
{ 
	PathName("test_date_and_time.odb").unlink();
}

static void test()
{
    const string sql = "select julian(date,time), year(date),month(date),day(date),hour(time),minute(time),second(time), timestamp(date,time) from \"test_date_and_time.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odc::Select oda(sql);
	odc::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 2455019.) < EPS); // 
	ASSERT((*it)[1] == 2009); // 
	ASSERT((*it)[2] == 7); // 
	ASSERT((*it)[3] == 6); // 
	ASSERT((*it)[4] == 21); // 
	ASSERT((*it)[5] == 1); // 
	ASSERT((*it)[6] == 9); // 
	ASSERT((*it)[7] == 20090706210109ll); // 

}



SIMPLE_TEST(FunctionDateAndTime)
