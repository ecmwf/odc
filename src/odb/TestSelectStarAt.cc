/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectStarAt.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestSelectStarAt.h"
#include "ToolFactory.h"

#define SRC __FILE__, __LINE__


#include "PathName.h"
#include "DataHandle.h"
#include "DataStream.h"
#include "HashTable.h"
#include "Codec.h"
#include "Column.h"
#include "MetaData.h"
#include "DataHandle.h"
#include "DataStream.h"
#include "RowsIterator.h"
#include "HashTable.h"
#include "Log.h"
#include "SQLBitfield.h"
#include "SQLAST.h"
#include "SchemaAnalyzer.h"
#include "SQLIteratorSession.h"
#include "Header.h"
#include "Reader.h"
#include "DataHandle.h"
#include "SelectIterator.h"
#include "ReaderIterator.h"
#include "oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSelectStarAt> _TestSelectStarAt("TestSelectStarAt");

TestSelectStarAt::TestSelectStarAt(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSelectStarAt::~TestSelectStarAt() { }

/// Tests expansion of '*@hdr' into a list of columns of the hdr ODB table.
///
void TestSelectStarAt::test()
{
	// TODO: make sure a 'select ... into ... from ...', e.g.:
	//
	//	const string SELECT = "select *@hdr into \"out.oda\" from \"../odb2oda/2000010106/ECMA.odb\";";
	//  is not returning a result set (iterator). Or perhaps it is returning an empty result set.

	const string SELECT = "select *@hdr from \"../odb2oda/2000010106/ECMA.odb\";";

	odb::Select oda(SELECT);

	Log::info() << "Executing: '" << SELECT << "'" << endl;
	odb::Select::iterator it = oda.begin();

	Log::info() << "it->columns().size() => " << it->columns().size() << endl;
	ASSERT(it->columns().size() == 27);

#if 0
	unsigned long long i = 0;
	for ( ; it != oda.end(); ++it) 
		++i;

	Log::info() << "i == " << i << endl;
	ASSERT(i == 3321753);
#endif
}

void TestSelectStarAt::setUp() {}

void TestSelectStarAt::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

