/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAggregateFunctions3.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include "odb_api/Select.h"
#include "odb_api/Reader.h"

#include "ImportTool.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

static void test()
{
	string sql = "select sum(a) from \"TestAggregateFunctions3.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odc::Select sel(sql);
	odc::Select::iterator it2 = sel.begin();
	odc::Select::iterator end2 = sel.end();

	ASSERT( (*it2)[0] == 55);
}

static void setUp()
{
	stringstream s;
	s << "a:REAL" << std::endl;
	for (size_t i = 1; i <= 10; ++i)
		s << i << std::endl;
    odc::tool::ImportTool::importText(s.str().c_str(), "TestAggregateFunctions3.odb");
}


static void tearDown(){}

SIMPLE_TEST(AggregateFunctions3)
