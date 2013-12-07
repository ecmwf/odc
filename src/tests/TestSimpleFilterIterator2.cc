/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include "eckit/log/Timer.h"
#include "odblib/Select.h"
#include "odblib/Reader.h"
#include "odblib/SimpleFilter.h"

#include "UnitTest.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {



/// UnitTest DispatchingWriter
///
void UnitTest::test()
{
	const string fileName = "2000010106.odb";
	string sql = string("select * from \"") + fileName + "\" where obstype = 7 and sensor = 1;";
	odb::Reader oda(fileName);
	odb::Select odas(sql, fileName);
	long n1 = 0;
	long n2 = 0;

	{
		Timer t("UnitTest::test: selecting rows where obstype == 7 and sensor = 1");
		typedef odb::SimpleFilter<odb::Reader::iterator> Filter;
		typedef odb::SimpleFilter<Filter::iterator> Filter2;

		Filter filterObstype(oda.begin(), oda.end(), "obstype", 7.0);
		Filter2 filterObstypeAndSensor(filterObstype.begin(), filterObstype.end(), "sensor", 1.0);

		Filter2::iterator it = filterObstypeAndSensor.begin();
		const Filter2::iterator end = filterObstypeAndSensor.end();

		for (; it != end; ++it)
			++n1;

		Log::info() << "UnitTest::test: selected " << n1 << " rows." << std::endl;
	}

	{
		Timer t("UnitTest::test: selecting rows using SQL where obstype == 7 and sensor = 1");
		Log::info() << "UnitTest::test: Execute '" << sql << "'" << std::endl;
		odb::Select::iterator end = odas.end();
		for(odb::Select::iterator it = odas.begin();
			it != end; ++it)
			++n2;
		Log::info() << "UnitTest::test: selected " << n2 << " rows." << std::endl;
	}

	Log::info() << "UnitTest::test: n1=" << n1 << ", n2=" << n2 << std::endl;

	ASSERT(n1 == n2);
}

void UnitTest::setUp() {}
void UnitTest::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 



