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

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileHandle.h"
#include "eckit/utils/StringTools.h"

#include "odc/api/Odc_cpp.h"
#include "odc/core/MetaData.h"
#include "odc/Select.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

///
static void test()
{
	{
		string sql = "select distinct a from \"TestOrderBy_a1to10twice.odb\" order by a;";

		odc::Select sel(sql);
		odc::Select::iterator it = sel.begin();
		odc::Select::iterator end = sel.end();

		int i = 0;
		for (; it != end; ++it)
		{
			int v = (*it)[0];
			ASSERT(v == ++i);
		}
		ASSERT(i == 10);
	}

	{
		string sql = "select a from \"TestOrderBy_a1to10twice.odb\" order by a;";

		odc::Select sel(sql);
		odc::Select::iterator it = sel.begin();
		odc::Select::iterator end = sel.end();

		int i = 0, j = 0;
		for (; it != end; ++it, ++j)
		{
			int v = (*it)[0];
			ASSERT(i <= v);
			i = v;
		}
		ASSERT(i == 10);
		ASSERT(j == 20);
	}

	{
		string sql = "select distinct a from \"TestOrderBy_a1to10twice.odb\" order by a desc;";

		odc::Select sel(sql);
		odc::Select::iterator it = sel.begin();
		odc::Select::iterator end = sel.end();

		int i = 10, j = 0;
		for (; it != end; ++it, ++j)
		{
			int v = (*it)[0];
			ASSERT(i-- == v);
		}
		ASSERT(i == 0);
		ASSERT(j == 10);
	}

	{
		const char *in =
		"a:REAL,b:REAL,c:STRING\n"
		"1,10,'one'\n"
		"1,20,'two'\n"
		"2,30,'three'\n"
		"2,40,'four'\n";
        {
            FileHandle dh("TestOrderBy.odb");
            dh.openForWrite(0);
            AutoClose close(dh);
            odc::api::importText(in, dh);
        }

		string sql = "select distinct a,b,c from \"TestOrderBy.odb\" order by a desc, b asc;";

		odc::Select sel(sql);
		odc::Select::iterator it = sel.begin();
		odc::Select::iterator end = sel.end();

        int i = 0, v1 = 0 , v2 = 0;
		string s;
		for (; it != end; ++it, ++i)
		{
			v1 = (*it)[0];
			v2 = (*it)[1];
			s = (*it).string(2);
		}
		ASSERT(i == 4);
		ASSERT(v1 == 1 && v2 == 20 && StringTools::trim(s) == "two");
	}
}


static void setUp()
{
	stringstream s;
	s << "a:REAL" << std::endl;
	for (size_t i = 1; i <= 10; ++i) s << i << std::endl;
	for (size_t i = 1; i <= 10; ++i) s << i << std::endl;
    FileHandle dh("TestOrderBy_a1to10twice.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::importText(s, dh);
}
static void tearDown(){}

SIMPLE_TEST(OrderBy)
