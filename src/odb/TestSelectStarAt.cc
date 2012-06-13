/*
 * (C) Copyright 1996-2012 ECMWF.
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

#include "odblib/oda.h"
#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestSelectStarAt.h"
#include "odblib/ToolFactory.h"




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
	//	const string SELECT = "select *@hdr into \"out.odb\" from \"2000010106.odb\";";
	//  is not returning a result set (iterator). Or perhaps it is returning an empty result set.

	const string SELECT = "select *@hdr from \"2000010106.odb\";";

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

