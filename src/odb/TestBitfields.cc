/// \file TestBitfields.h
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
#include "TestBitfields.h"
#include "ToolFactory.h"
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

#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestBitfields> _TestBitfields("TestBitfields");

TestBitfields::TestBitfields(int argc, char **argv)
: TestCase(argc, argv)
{}

TestBitfields::~TestBitfields() { }

/// Tests problem fixed with p4 change 23687
///
void TestBitfields::test()
{
	//const string SELECT = "select status@body, status.*@body from \"../odb2oda/2000010106/ECMA.odb\";";AA
	string statusFields =
"status.active@body,status.passive@body,status.rejected@body,status.blacklisted@body,status.monthly@body,status.constant@body,status.experimental@body,status.whitelist@body";

	statusFields = "status.*@body";
	const string SELECT = string("select status@body, ") + statusFields + " from \"../odb2oda/2000010106/ECMA.odb\";";

	Log::info() << "Executing '" << SELECT << "'" << endl;

	odb::Select oda(SELECT);
	long int i=0;

	odb::Select::iterator it = oda.begin();

	Log::debug() << "TestBitfields::test: it->columns().size() == " << it->columns().size() << endl;

	ASSERT(it->columns().size() == 9);

	for ( ; it != oda.end() && i < 5000; ++it, ++i) 
	{
		unsigned int sum =
			  int ((*it)[1])
			| int ((*it)[2]) << 1
			| int ((*it)[3]) << 2
			| int ((*it)[4]) << 3
			| int ((*it)[5]) << 4
			| int ((*it)[6]) << 5
			| int ((*it)[7]) << 6
			| int ((*it)[8]) << 7;
		//Log::info() << i << ": " << (*it)[0] << " " << sum << endl;
		ASSERT((*it)[0] == sum);
	}
}

void TestBitfields::setUp() {}

void TestBitfields::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

