/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAggregateFunctions.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "odblib/oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestAggregateFunctions.h"
#include "ToolFactory.h"

#define SRC __FILE__, __LINE__


#include "eclib/PathName.h"
#include "eclib/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/MetaData.h"
#include "eclib/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/RowsIterator.h"
#include "odblib/HashTable.h"
#include "eclib/Log.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/Header.h"
#include "odblib/Reader.h"
#include "eclib/DataHandle.h"
#include "odblib/SelectIterator.h"
#include "odblib/ReaderIterator.h"
#include "odblib/oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestAggregateFunctions> _TestAggregateFunctions("TestAggregateFunctions");

TestAggregateFunctions::TestAggregateFunctions(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAggregateFunctions::~TestAggregateFunctions() { }

///
void TestAggregateFunctions::test()
{
	string sql = readFile("TestAggregateFunctions.sql");

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	//Log::info() << "it->columns().size() => " << it->columns().size() << endl;
	ASSERT(it->columns().size() == 14);
	ASSERT((*it)[0] == 91119); // COUNT(*) == 91119
	ASSERT((*it)[1] == 91119); // COUNT(lat) == 91119
}

void TestAggregateFunctions::setUp() {}

void TestAggregateFunctions::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

