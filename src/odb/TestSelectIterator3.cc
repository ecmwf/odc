/// \file TestSelectIterator3.h
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
#include "TestSelectIterator3.h"
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

ToolFactory<TestSelectIterator3> _TestSelectIterator3("TestSelectIterator3");

TestSelectIterator3::TestSelectIterator3(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSelectIterator3::~TestSelectIterator3() { }

const string testFile = "TestSelectIterator3.oda";

const double VALUE[] = { 1, 2, 3 };

void TestSelectIterator3::setUp()
{
	Log::debug() << "TestSelectIterator3::setUp" << endl;

	Timer t("Writing " + testFile );
	odb::Writer<> oda(testFile);

	odb::Writer<>::iterator writer = oda.begin();
	writer->columns().setSize(1);
	(**writer).setColumn(0, "value", odb::INTEGER);
	(**writer).writeHeader();

	for (size_t i = 0; i < sizeof(VALUE) / sizeof(double); ++i)
	{
		(*writer)[0] = VALUE[i]; // col 0
		++writer;
	}
}

///
/// Tests problem fixed with p4 change 23687
///
void TestSelectIterator3::test()
{
	const string SELECT = "select * from \"" + testFile + "\";";

	odb::Select oda(SELECT);
	size_t i=0;
	for (odb::Select::iterator it = oda.begin();
		it != oda.end() && i < sizeof(VALUE) / sizeof(double);
		++it, ++i) 
	{
		Log::info() << "TestSelectIterator3::testBug01: it[" << i << "]=" << (*it)[0] << ", should be " << VALUE[i] << endl;
		ASSERT((*it)[0] == VALUE[i]);
	}
}

void TestSelectIterator3::tearDown() { }

} // namespace test
} // namespace tool 
} // namespace odb 

