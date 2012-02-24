/// \file TestAggregateFunctions3.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestAggregateFunctions3.h"
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
#include "FILEHandle.h"
#include "SelectIterator.h"
#include "ReaderIterator.h"
#include "oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestAggregateFunctions3> _TestAggregateFunctions3("TestAggregateFunctions3");

TestAggregateFunctions3::TestAggregateFunctions3(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAggregateFunctions3::~TestAggregateFunctions3() { }

///
void TestAggregateFunctions3::test()
{
	string sql = "select sum(a) from \"TestAggregateFunctions3.odb\"";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	ASSERT( (*it2)[0] == 55);
}

void TestAggregateFunctions3::setUp() {}

void TestAggregateFunctions3::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

