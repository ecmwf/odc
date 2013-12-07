/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionsForTemperatureConversion.cc
///
/// @author ECMWF, July 2010

#include "eckit/utils/Timer.h"
#include "odblib/Select.h"
#include "odblib/ToolFactory.h"
#include "odblib/Writer.h"
#include "TestFunctionsForTemperatureConversion.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionsForTemperatureConversion> _TestFunctionsForTemperatureConversion("TestFunctionsForTemperatureConversion");

TestFunctionsForTemperatureConversion::TestFunctionsForTemperatureConversion(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionsForTemperatureConversion::~TestFunctionsForTemperatureConversion() { }

void TestFunctionsForTemperatureConversion::test()
{
	testReaderIterator();
}

void TestFunctionsForTemperatureConversion::setUp()
{
	Timer t("Test various functions to convert temperatures");
	odb::Writer<> oda("test_tempconv.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(3);

	row->setColumn(0, "kelvin_col", odb::REAL);
	row->setColumn(1, "celsius_col", odb::REAL);
	row->setColumn(2, "fahrenheit_col", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 273.15;
	(*row)[1] = 0.0;
	(*row)[2] = 32;
	++row;
}

void TestFunctionsForTemperatureConversion::tearDown() 
{ 
	PathName("test_tempconv.odb").unlink();
}

void TestFunctionsForTemperatureConversion::testReaderIterator()
{
    const string sql = "select celsius(kelvin_col), fahrenheit(kelvin_col), c2k(celsius_col),c2f(celsius_col),f2c(fahrenheit_col), f2k(fahrenheit_col), k2f(kelvin_col) from \"test_tempconv.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT((*it)[0] == 0.0); // celsius(273.15) = 0.0
	ASSERT((*it)[1] == 32); // farhenheit(273.15) = 31.73
	ASSERT((*it)[2] == 273.15); // c2k
	ASSERT((*it)[3] == 32);    // c2f

	ASSERT((*it)[4] == 0); // f2c
	ASSERT((*it)[5] == 273.15);    // f2k
	ASSERT((*it)[6] == 32);    // k2f

}


} // namespace test 
} // namespace tool 
} // namespace odb 

