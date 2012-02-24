/// \file TestAggregateFunctions.h
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
#include "TestAggregateFunctions.h"
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

ToolFactory<TestAggregateFunctions> _TestAggregateFunctions("TestAggregateFunctions");

TestAggregateFunctions::TestAggregateFunctions(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAggregateFunctions::~TestAggregateFunctions() { }

///
void TestAggregateFunctions::test()
{
	string sql = readFile("TestAggregateFunctions.sql");

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	//Log::info() << "it->columns().size() => " << it->columns().size() << endl;
	ASSERT(it->columns().size() == 14);
	ASSERT((*it)[0] == 91119); // COUNT(*) == 91119
	ASSERT((*it)[1] == 91119); // COUNT(lat) == 91119
}

void TestAggregateFunctions::setUp() {}

void TestAggregateFunctions::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

