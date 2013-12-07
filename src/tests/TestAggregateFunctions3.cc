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
/// @author Piotr Kuchta, ECMWF, September 2010

#include "tools/ImportTool.h"
#include "odblib/Select.h"
#include "odblib/Reader.h"

#include "tests/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void test()
{
	string sql = "select sum(a) from \"UnitTest.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	ASSERT( (*it2)[0] == 55);
}

static void setUp()
{
	stringstream s;
	s << "a:REAL" << std::endl;
	for (size_t i = 1; i <= 10; ++i)
		s << i << std::endl;
    odb::tool::ImportTool::importText(s.str().c_str(), "UnitTest.odb");
}


static void tearDown(){}

TEST_MAIN;
