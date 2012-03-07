/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCatFiles.h
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
#include "TestCatFiles.h"
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

namespace odb {
namespace tool {
namespace test {


ToolFactory<TestCatFiles> testCatFiles("TestCatFiles");

TestCatFiles::TestCatFiles(int argc, char **argv)
: TestCase(argc, argv)
{}

TestCatFiles::~TestCatFiles() { }

void TestCatFiles::test()
{
	testSelectIterator();
}

void TestCatFiles::setUp()
{
 {
	odb::Writer<> oda("file1.oda");
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

	odb::Writer<> oda2("file2.oda");
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

	odb::Writer<> oda3("file3.oda");
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

	int catStatus = system("cat file1.oda file2.oda file3.oda >concatenated.oda");
	ASSERT(WEXITSTATUS(catStatus) == 0);

}

void TestCatFiles::tearDown() { }

void TestCatFiles::testSelectIterator()
{
	const string sql = "select X,Y from \"concatenated.oda\";";
	const string fileName = "concatenated.oda";
	odb::Select oda(sql, fileName);
	Log::info() << "Iterating " << sql << endl;

	//for (Reader::iterator it = oda.begin();
	//	it != oda.end();
	//	++it)
	//{}

	odb::Select::iterator it = oda.begin();
	int j = 1;

	Log::info(SRC) << "j = " << j << endl;
	const double * data = it->data();
	ASSERT(data);
	Log::info(SRC) << "data[0] = " << data[0] << endl;
	Log::info(SRC) << "data[1] = " << data[1] << endl;
	ASSERT(data[0] == j);
	ASSERT(data[1] == j);
	++j;

	++it;
	Log::info(SRC) << "j = " << j << endl;
	data = it->data();
	ASSERT(data);
	Log::info(SRC) << "data[0] = " << data[0] << endl;
	Log::info(SRC) << "data[1] = " << data[1] << endl;
	ASSERT(data[0] == j);
	ASSERT(data[1] == j);
	++j;

	j = 1; // data from file2.oda

	++it;
	Log::info(SRC) << "j = " << j << endl;
	data = it->data();
	ASSERT(data);
	Log::info(SRC) << "data[0] = " << data[0] << endl;
	Log::info(SRC) << "data[1] = " << data[1] << endl;
	ASSERT(data[0] == j * 10);
	ASSERT(data[1] == j * 100);
	++j;

	++it;
	Log::info(SRC) << "j = " << j << endl;
	data = it->data();
	ASSERT(data);
	Log::info(SRC) << "data[0] = " << data[0] << endl;
	Log::info(SRC) << "data[1] = " << data[1] << endl;
	ASSERT(data[0] == j * 10);
	ASSERT(data[1] == j * 100);


	j = 1; // data from file3.oda

	++it;
	Log::info(SRC) << "j = " << j << endl;
	data = it->data();
	ASSERT(data);
	Log::info(SRC) << "data[0] = " << data[0] << endl;
	Log::info(SRC) << "data[1] = " << data[1] << endl;
	ASSERT(data[0] == j * 10);
	ASSERT(data[1] == j * 100);
	++j;

	++it;
	Log::info(SRC) << "j = " << j << endl;
	data = it->data();
	ASSERT(data);
	Log::info(SRC) << "data[0] = " << data[0] << endl;
	Log::info(SRC) << "data[1] = " << data[1] << endl;
	ASSERT(data[0] == j * 10);
	ASSERT(data[1] == j * 100);
}

} // namespace test 
} // namespace tool 
} // namespace odb 

