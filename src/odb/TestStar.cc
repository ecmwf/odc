/// \file TestStar.h
///
/// @author Piotr Kuchta, ECMWF, May 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestStar.h"
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

ToolFactory<TestStar> _TestStar("TestStar");

TestStar::TestStar(int argc, char **argv)
: TestCase(argc, argv)
{}

TestStar::~TestStar() { }

/// Tests syntax: select *@odb_table from "file.oda";
///
void TestStar::test()
{
	const string SELECT = "select *@hdr from \"../odb2oda/2000010106/ECMA.odb\";";

	odb::Select oda(SELECT);

	odb::Select::iterator it = oda.begin();
	ASSERT("hdr has 27 columns excluding @LINKs." && it->columns().size() == 27);
}

void TestStar::setUp() {}

void TestStar::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

