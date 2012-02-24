/// \file TestDistinct.h
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
#include "TestDistinct.h"
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

ToolFactory<TestDistinct> _TestDistinct("TestDistinct");

TestDistinct::TestDistinct(int argc, char **argv)
: TestCase(argc, argv)
{}

TestDistinct::~TestDistinct() { }

///
void TestDistinct::test()
{
	string sql = "select distinct a from \"a1to10twice.odb\"";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	int i = 0;
	for (; it2 != end2; ++it2)
		ASSERT((*it2)[0] == ++i);

	ASSERT((*it2)[0] == 10);
}

void TestDistinct::setUp() {}

void TestDistinct::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

