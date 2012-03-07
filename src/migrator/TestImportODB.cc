/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestImportODB.h
///
/// @author Piotr Kuchta, ECMWF, March 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "oda.h"
#include "PathName.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestImportODB.h"
#include "ToolFactory.h"
#include "SchemaAnalyzer.h"
#include "SQLInteractiveSession.h"
#include "ImportODBTool.h"
#include "ODBIterator.h"

#define SRC __FILE__, __LINE__

#include "oda.h"

namespace odb {
namespace tool {

ToolFactory<TestImportODB> _TestImportODB("TestImportODB");

TestImportODB::TestImportODB(int argc, char **argv)
: TestCase(argc, argv)
{}

TestImportODB::~TestImportODB() { }

void TestImportODB::setUp() { }

void TestImportODB::tearDown() { }

void TestImportODB::test()
{
	const char *argv[] = {"importodb", "ECMA/ECMA.reo3", "ECMA/odbdump_reo3.sql", "ECMA.reo3.odb", 0 };
	ImportODBTool<> importer(4, const_cast<char **>(argv));
	importer.run();
}

} // namespace tool 
} // namespace odb 

