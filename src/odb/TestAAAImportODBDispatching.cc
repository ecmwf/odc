/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAAAImportODBDispatching.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/io/DataHandle.h"

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

#include "odb/TestAAAImportODBDispatching.h"

using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestAAAImportODBDispatching> _TestAAAImportODBDispatching("TestAAAImportODBDispatching");

TestAAAImportODBDispatching::TestAAAImportODBDispatching(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAAAImportODBDispatching::~TestAAAImportODBDispatching() {}

void TestAAAImportODBDispatching::test()
{
	if (getenv("ODB_ROOT"))
		shell("MALLOC_CHECK_=2 ODB_COMPILER_FLAGS=`pwd`/2000010106/ECMA/ECMA.flags ./odb_migrator 2000010106/ECMA . 2000010106.{obstype}.{sensor}.odb", Here());
	else {
		Log::warning() << "TestAAAImportODBDispatching: ODB_ROOT not set, skipping testing of odb_migrator" << endl;
		shell("./odb split 2000010106.odb 2000010106.{obstype}.{sensor}.odb", Here());
	}
}

void TestAAAImportODBDispatching::setUp() {}

void TestAAAImportODBDispatching::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

