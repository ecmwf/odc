/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAAAImportODB.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;


#define SRC __FILE__, __LINE__


#include "eclib/PathName.h"
#include "eclib/Log.h"
#include "eclib/DataHandle.h"

#include "odb/Tool.h"
#include "odb/TestCase.h"
#include "odb/TestAAAImportODB.h"
#include "odb/ToolFactory.h"

#include "odblib/oda.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
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

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestAAAImportODB> _TestAAAImportODB("TestAAAImportODB");

TestAAAImportODB::TestAAAImportODB(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAAAImportODB::~TestAAAImportODB() {}

void TestAAAImportODB::test()
{
	ksh("cd ../odb2oda && ./odb2oda era40:2000010106", SRC);
}


void TestAAAImportODB::setUp() {}

void TestAAAImportODB::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

