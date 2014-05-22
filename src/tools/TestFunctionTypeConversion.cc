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

#include "eckit/log/Timer.h"
#include "odb_api/odblib/Select.h"

#include "odb_api/odblib/Writer.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;


static void setUp()
{
	Timer t("Test TypeConversion function");
	odb::Writer<> oda("test_type_conversion.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->setNumberOfColumns(1);

	row->setColumn(0, "obsvalue", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 247.53;

    ++row;
}

static void tearDown() 
{ 
	PathName("test_type_conversion.odb").unlink();
}

static void test()
{
    const string sql = "select ceil(obsvalue),floor(obsvalue), trunc(obsvalue),int(obsvalue),nint(obsvalue) from \"test_type_conversion.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT((*it)[0] == 248); // 
	ASSERT((*it)[1] == 247); // 
	ASSERT((*it)[2] == 247); // 
	ASSERT((*it)[3] == 247); // 
	ASSERT((*it)[4] == 248); // 

}




SIMPLE_TEST(FunctionTypeConversion)
