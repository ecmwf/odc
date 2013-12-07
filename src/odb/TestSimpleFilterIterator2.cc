/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSimpleFilterIterator2.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include "eckit/utils/Timer.h"
#include "odblib/Select.h"
#include "odblib/Reader.h"
#include "odblib/SimpleFilter.h"
#include "odblib/ToolFactory.h"
#include "odb/TestSimpleFilterIterator2.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSimpleFilterIterator2> _TestSimpleFilterIterator2("TestSimpleFilterIterator2");

TestSimpleFilterIterator2::TestSimpleFilterIterator2(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSimpleFilterIterator2::~TestSimpleFilterIterator2() { }

/// Tests DispatchingWriter
///
void TestSimpleFilterIterator2::test()
{
	const string fileName = "2000010106.odb";
	string sql = string("select * from \"") + fileName + "\" where obstype = 7 and sensor = 1;";
	odb::Reader oda(fileName);
	odb::Select odas(sql, fileName);
	long n1 = 0;
	long n2 = 0;

	{
		Timer t("TestSimpleFilterIterator2::test: selecting rows where obstype == 7 and sensor = 1");
		typedef odb::SimpleFilter<odb::Reader::iterator> Filter;
		typedef odb::SimpleFilter<Filter::iterator> Filter2;

		Filter filterObstype(oda.begin(), oda.end(), "obstype", 7.0);
		Filter2 filterObstypeAndSensor(filterObstype.begin(), filterObstype.end(), "sensor", 1.0);

		Filter2::iterator it = filterObstypeAndSensor.begin();
		const Filter2::iterator end = filterObstypeAndSensor.end();

		for (; it != end; ++it)
			++n1;

		Log::info() << "TestSimpleFilterIterator2::test: selected " << n1 << " rows." << std::endl;
	}

	{
		Timer t("TestSimpleFilterIterator2::test: selecting rows using SQL where obstype == 7 and sensor = 1");
		Log::info() << "TestSimpleFilterIterator2::test: Execute '" << sql << "'" << std::endl;
		odb::Select::iterator end = odas.end();
		for(odb::Select::iterator it = odas.begin();
			it != end; ++it)
			++n2;
		Log::info() << "TestSimpleFilterIterator2::test: selected " << n2 << " rows." << std::endl;
	}

	Log::info() << "TestSimpleFilterIterator2::test: n1=" << n1 << ", n2=" << n2 << std::endl;

	ASSERT(n1 == n2);
}

void TestSimpleFilterIterator2::setUp() {}
void TestSimpleFilterIterator2::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

