/// \file TestAAAImportODB.h
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
#include "TestAAAImportODB.h"
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

