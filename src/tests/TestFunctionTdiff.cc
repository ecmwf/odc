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

//const double EPS =     7e-6

#include "eckit/log/Timer.h"
#include "odblib/Select.h"

#include "odblib/Writer.h"
#include "tests/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;



static void setUp()
{
	Timer t("Test tdiff function");
	odb::Writer<> oda("test_tdiff.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "date", odb::INTEGER);
	row->setColumn(1, "time", odb::INTEGER);
	
	row->writeHeader();

	(*row)[0] = 20090706.0;
	(*row)[1] = 210109.0;

    ++row;
}

static void tearDown() 
{ 
	PathName("test_tdiff.odb").unlink();
}

static void test()
{
    const string sql = "select tdiff(date,time,20090707.0,0.0) from \"test_tdiff.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT((*it)[0] == -10731); // 

}



TEST_MAIN;
