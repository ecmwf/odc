/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestDistinct.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "odblib/oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestDistinct.h"
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
#include "eclib/FILEHandle.h"
#include "odblib/SelectIterator.h"
#include "odblib/ReaderIterator.h"
#include "odblib/oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestDistinct> _TestDistinct("TestDistinct");

TestDistinct::TestDistinct(int argc, char **argv)
: TestCase(argc, argv)
{}

TestDistinct::~TestDistinct() { }

///
void TestDistinct::test()
{
	string sql = "select distinct a from \"a1to10twice.odb\"";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	int i = 0;
	for (; it2 != end2; ++it2)
		ASSERT((*it2)[0] == ++i);

	ASSERT((*it2)[0] == 10);
}

void TestDistinct::setUp() {}

void TestDistinct::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

