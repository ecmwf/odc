/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAggregateFunctions2.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/filesystem/StdFileHandle.h"

#include "odblib/odb_api.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Header.h"
#include "odblib/MetaData.h"
#include "odblib/Reader.h"
#include "odblib/ReaderIterator.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SelectIterator.h"
#include "odblib/TestCase.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/ImportTool.h"

#include "odb/TestAggregateFunctions2.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestAggregateFunctions2> _TestAggregateFunctions2("TestAggregateFunctions2");

TestAggregateFunctions2::TestAggregateFunctions2(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAggregateFunctions2::~TestAggregateFunctions2() { }

///
void TestAggregateFunctions2::test()
{
	string sql = "select count(*) from \"TestAggregateFunctions2.odb\"";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(it->columns().size() == 1);
	ASSERT((*it)[0] == 10); 

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	FILE *fout = fopen("TestAggregateFunctions2_out.odb", "w");
	StdFileHandle fhout(fout);
	odb::Writer<> writer(fhout);
	odb::Writer<>::iterator outit = writer.begin();

	//outit->pass1(it2, end2);
	size_t i = 0;
	for (; it2 != end2; ++it2)
	{
		++i;
		ASSERT( (*it2)[0] == 10);
	}
	ASSERT( i == 1);
}

void TestAggregateFunctions2::setUp()
{
	stringstream s;
	s << "a:REAL" << endl;
	for (size_t i = 1; i <= 10; ++i)
		s << i << endl;
	ImportTool::importText(s.str().c_str(), "TestAggregateFunctions2.odb");
}

void TestAggregateFunctions2::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

