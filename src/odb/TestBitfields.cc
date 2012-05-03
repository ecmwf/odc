/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestBitfields.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "eclib/PathName.h"
#include "eclib/Log.h"
#include "eclib/DataHandle.h"

#include "odblib/oda.h"
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

#include "odb/TestBitfields.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestBitfields> _TestBitfields("TestBitfields");

TestBitfields::TestBitfields(int argc, char **argv)
: TestCase(argc, argv)
{}

TestBitfields::~TestBitfields() { }

/// Tests problem fixed with p4 change 23687
///
void TestBitfields::test()
{
	string statusFields =
"status.active@body,status.passive@body,status.rejected@body,status.blacklisted@body,status.monthly@body,status.constant@body,status.experimental@body,status.whitelist@body";

	statusFields = "status.*@body";
	const string SELECT = string("select status@body, ") + statusFields + " from \"2000010106/ECMA.odb\";";

	Log::info() << "Executing '" << SELECT << "'" << endl;

	odb::Select oda(SELECT);
	long int i=0;

	odb::Select::iterator it = oda.begin();

	Log::debug() << "TestBitfields::test: it->columns().size() == " << it->columns().size() << endl;

	ASSERT(it->columns().size() == 9);

	for ( ; it != oda.end() && i < 5000; ++it, ++i) 
	{
		unsigned int sum =
			  int ((*it)[1])
			| int ((*it)[2]) << 1
			| int ((*it)[3]) << 2
			| int ((*it)[4]) << 3
			| int ((*it)[5]) << 4
			| int ((*it)[6]) << 5
			| int ((*it)[7]) << 6
			| int ((*it)[8]) << 7;
		//Log::info() << i << ": " << (*it)[0] << " " << sum << endl;
		ASSERT((*it)[0] == sum);
	}
}

void TestBitfields::setUp() {}

void TestBitfields::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

