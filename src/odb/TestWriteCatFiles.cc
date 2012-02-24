/// \file TestWriteCatFiles.h
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
#include "TestWriteCatFiles.h"
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
#include "Comparator.h"
#include "oda.h"

namespace odb {
namespace tool {
namespace test {


ToolFactory<TestWriteCatFiles> testWriteCatFiles("TestWriteCatFiles");

TestWriteCatFiles::TestWriteCatFiles(int argc, char **argv)
: TestCase(argc, argv)
{}

TestWriteCatFiles::~TestWriteCatFiles() { }

void TestWriteCatFiles::test()
{
	Reader in("concatenated.odb");
	Reader::iterator it = in.begin();
	Reader::iterator end = in.end();

	Writer<> out("copy_of_concatenated.odb");
	Writer<>::iterator o = out.begin();
	o->pass1(it, end);

	Comparator().compare("concatenated.odb", "copy_of_concatenated.odb");
}

void TestWriteCatFiles::setUp()
{
 {
	odb::Writer<> oda("file1.odb");
	odb::Writer<>::iterator row = oda.begin();

	MetaData& md = row->columns();
	md.setSize(3);

	row->setColumn(0, "x", odb::REAL);
	row->setColumn(1, "y", odb::REAL);
	row->setColumn(2, "z", odb::REAL);
	
	row->writeHeader();

	for (size_t i = 1; i <= 2; i++)
	{
		(*row)[0] = i; // col 0
		(*row)[1] = i; // col 1
		(*row)[2] = i; // col 2
		++row;
	}

	odb::Writer<> oda2("file2.odb");
	odb::Writer<>::iterator row2 = oda2.begin();
	row2->columns().setSize(3);
	row2->setColumn(0, "x", odb::REAL);
	row2->setColumn(1, "y", odb::REAL);
	row2->setColumn(2, "v", odb::REAL);
	
	row2->writeHeader();

	for (size_t i = 1; i <= 2; i++)
	{
		(*row2)[0] = i * 10; // col 0
		(*row2)[1] = i * 100; // col 1
		(*row2)[2] = i * 1000; // col 2
		++row2;
	}

	odb::Writer<> oda3("file3.odb");
	odb::Writer<>::iterator row3 = oda3.begin();
	row3->columns().setSize(4);

	row3->setColumn(0, "x", odb::REAL);
	row3->setColumn(1, "v", odb::REAL);
	row3->setColumn(2, "y", odb::REAL);
	row3->setColumn(3, "z", odb::REAL);
	row3->writeHeader();

	for (size_t i = 1; i <= 2; i++)
	{
		(*row3)[0] = i * 10; // col 0
		(*row3)[1] = i * 1000; // col 1
		(*row3)[2] = i * 100; // col 2
		(*row3)[3] = 13;     // col 3
		++row3;
	}
	// dtors fire here
 }

	int catStatus = system("cat file1.odb file2.odb file3.odb >concatenated.odb");
	ASSERT(WEXITSTATUS(catStatus) == 0);

}

void TestWriteCatFiles::tearDown() { }


} // namespace test 
} // namespace tool 
} // namespace odb 

