/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestStar.h
///
/// @author Piotr Kuchta, ECMWF, May 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;



#include "eclib/DataHandle.h"
#include "eclib/DataHandle.h"
#include "eclib/DataHandle.h"
#include "eclib/Log.h"
#include "eclib/PathName.h"

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odb/TestStar.h"
#include "odblib/ToolFactory.h"

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

ToolFactory<TestStar> _TestStar("TestStar");

TestStar::TestStar(int argc, char **argv)
: TestCase(argc, argv)
{}

TestStar::~TestStar() { }

/// Tests syntax: select *@odb_table from "file.oda";
///
void TestStar::test()
{
	const string SELECT = "select *@hdr from \"2000010106.odb\";";

	odb::Select oda(SELECT);

	odb::Select::iterator it = oda.begin();
	ASSERT("hdr has 27 columns excluding @LINKs." && it->columns().size() == 27);
}

void TestStar::setUp() {}

void TestStar::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

