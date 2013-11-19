/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestOrderBy.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "odblib/odb_api.h"
#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestOrderBy.h"
#include "odblib/ToolFactory.h"




#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/MetaData.h"
#include "eckit/io/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/RowsIterator.h"
#include "odblib/HashTable.h"
#include "eckit/log/Log.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/Header.h"
#include "odblib/Reader.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/StdFileHandle.h"
#include "odblib/SelectIterator.h"
#include "odblib/ReaderIterator.h"
#include "odblib/odb_api.h"
#include "odblib/ImportTool.h"
#include "odblib/StringTool.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestOrderBy> _TestOrderBy("TestOrderBy");

TestOrderBy::TestOrderBy(int argc, char **argv)
: TestCase(argc, argv)
{}

TestOrderBy::~TestOrderBy() {}

///
void TestOrderBy::test()
{
	{
		string sql = "select distinct a from \"a1to10twice.odb\" order by a;";
		Log::info() << "Executing: '" << sql << "'" << endl;

		odb::Select sel(sql);
		odb::Select::iterator it = sel.begin();
		odb::Select::iterator end = sel.end();

		Log::info()  << "TestOrderBy::test: entering the loop" << endl;
		int i = 0;
		for (; it != end; ++it)
		{
			int v = (*it)[0];
			Log::info()  << "TestOrderBy::test:" <<  v  << endl;
			ASSERT(v == ++i);
		}
		Log::info()  << "TestOrderBy::test: i = " <<  i  << endl;
		ASSERT(i == 10);
	}

	{
		string sql = "select a from \"a1to10twice.odb\" order by a;";
		Log::info() << "Executing: '" << sql << "'" << endl;

		odb::Select sel(sql);
		odb::Select::iterator it = sel.begin();
		odb::Select::iterator end = sel.end();

		Log::info()  << "TestOrderBy::test: entering the loop" << endl;
		int i = 0, j = 0;
		for (; it != end; ++it, ++j)
		{
			int v = (*it)[0];
			Log::info()  << "TestOrderBy::test:" <<  v  << endl;
			ASSERT(i <= v);
			i = v;
		}
		Log::info()  << "TestOrderBy::test: i=" <<  i  << ", j=" << j << endl;
		ASSERT(i == 10);
		ASSERT(j == 20);
	}

	{
		string sql = "select distinct a from \"a1to10twice.odb\" order by a desc;";
		Log::info() << "Executing: '" << sql << "'" << endl;

		odb::Select sel(sql);
		odb::Select::iterator it = sel.begin();
		odb::Select::iterator end = sel.end();

		Log::info()  << "TestOrderBy::test: entering the loop" << endl;
		int i = 10, j = 0;
		for (; it != end; ++it, ++j)
		{
			int v = (*it)[0];
			Log::info()  << "TestOrderBy::test:" <<  v  << endl;
			ASSERT(i-- == v);
		}
		Log::info()  << "TestOrderBy::test: i = " <<  i  << endl;
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
		ImportTool::importText(in, "TestOrderBy4.odb");

		string sql = "select distinct a,b,c from \"TestOrderBy4.odb\" order by a desc, b asc;";
		Log::info() << "Executing: '" << sql << "'" << endl;

		odb::Select sel(sql);
		odb::Select::iterator it = sel.begin();
		odb::Select::iterator end = sel.end();

		Log::info()  << "TestOrderBy::test: entering the loop" << endl;
        int i = 0, v1 = 0 , v2 = 0;
		string s;
		for (; it != end; ++it, ++i)
		{
			v1 = (*it)[0];
			v2 = (*it)[1];
			s = (*it).string(2);
			Log::info() << "TestOrderBy::test:" <<  v1  << ", " << v2 << ", '" << s << "'" << endl;
		}
		Log::info()  << "TestOrderBy::test: i = " <<  i  << endl;
		ASSERT(i == 4);
		ASSERT(v1 == 1 && v2 == 20 && StringTools::trim(s) == "two");
	}
}

void TestOrderBy::setUp() {}

void TestOrderBy::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

