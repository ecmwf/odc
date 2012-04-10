/*
 * © Copyright 1996-2012 ECMWF.
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

#include "odblib/oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestAAAImportODBDispatching.h"
#include "ToolFactory.h"




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

ToolFactory<TestAAAImportODBDispatching> _TestAAAImportODBDispatching("TestAAAImportODBDispatching");

TestAAAImportODBDispatching::TestAAAImportODBDispatching(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAAAImportODBDispatching::~TestAAAImportODBDispatching() {}

void TestAAAImportODBDispatching::test()
{
    ksh("cd ../odb2oda && ./odb2oda era40:2000010106 . 2000010106.{obstype}.{sensor}.oda", Here());
}

void TestAAAImportODBDispatching::setUp() {}

void TestAAAImportODBDispatching::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

