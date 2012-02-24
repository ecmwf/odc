/// \file TestAAAImportODBDispatching.h
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
#include "TestAAAImportODBDispatching.h"
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

ToolFactory<TestAAAImportODBDispatching> _TestAAAImportODBDispatching("TestAAAImportODBDispatching");

TestAAAImportODBDispatching::TestAAAImportODBDispatching(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAAAImportODBDispatching::~TestAAAImportODBDispatching() {}

void TestAAAImportODBDispatching::test()
{
	ksh("cd ../odb2oda && ./odb2oda era40:2000010106 . 2000010106.{obstype}.{sensor}.oda", SRC);
}

void TestAAAImportODBDispatching::setUp() {}

void TestAAAImportODBDispatching::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

