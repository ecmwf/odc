/// \file TestSetvbuffer.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestSetvbuffer.h"
#include "ToolFactory.h"

#define SRC __FILE__, __LINE__

#include "PathName.h"
#include "DataHandle.h"
#include "DataStream.h"
#include "HashTable.h"
#include "Codec.h"
#include "HashTable.h"
#include "Column.h"
#include "MetaData.h"
#include "RowsIterator.h"
#include "HashTable.h"
#include "Log.h"
#include "SQLBitfield.h"
#include "SQLAST.h"
#include "SchemaAnalyzer.h"
#include "SQLIteratorSession.h"
#include "Header.h"
#include "Reader.h"
#include "SelectIterator.h"
#include "ReaderIterator.h"
#include "oda.h"
#include "ODBAPISettings.h"


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSetvbuffer> testSetvbuffer("TestSetvbuffer");

TestSetvbuffer::TestSetvbuffer(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSetvbuffer::~TestSetvbuffer() { }

void TestSetvbuffer::setUp() { }

void TestSetvbuffer::test()
{
	size_t cols = 400;
	long long rows = 1000;
	//size_t buffSize = MEGA(8);
	size_t buffSize = 8 * 1024 * 1024;

	for (size_t i = 0; i < 10; ++i)
	{
		stringstream s;
		s << "TestSetvbuffer::setUp(): createFile(" << cols << ", " << rows << ", " << buffSize << ")" << endl;
		Timer t(s.str());
		createFile(cols, rows, buffSize);
	}
}

void TestSetvbuffer::createFile(size_t numberOfColumns, long long numberOfRows, size_t setvbufferSize)
{

	ODBAPISettings::instance().setvbufferSize(setvbufferSize);

	odb::Writer<> oda("TestSetvbuffer.odb");
	odb::Writer<>::iterator row = oda.begin();

	MetaData& md = row->columns();
	md.setSize(numberOfColumns);
	for (size_t i = 0; i < numberOfColumns; ++i)
	{
		stringstream name;
		name << "Column" << i;
		row->setColumn(i, name.str().c_str(), odb::REAL);
	}
	row->writeHeader();

	for (long long i = 1; i <= numberOfRows; ++i, ++row)
		for (size_t c = 0; c < numberOfColumns; ++c)
			(*row)[c] = c;
}

void TestSetvbuffer::tearDown()
{
	int catStatus = system("ls -l TestSetvbuffer.odb");
	ASSERT(WEXITSTATUS(catStatus) == 0);
}


} // namespace test 
} // namespace tool 
} // namespace odb 

