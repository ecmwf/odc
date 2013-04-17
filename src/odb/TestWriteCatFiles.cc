/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestWriteCatFiles.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>

using namespace std;

#include "odblib/odb_api.h"
#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestWriteCatFiles.h"
#include "odblib/ToolFactory.h"




#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/HashTable.h"
#include "odblib/Column.h"
#include "odblib/MetaData.h"
#include "odblib/RowsIterator.h"
#include "odblib/HashTable.h"
#include "eckit/log/Log.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/Header.h"
#include "odblib/Reader.h"
#include "odblib/SelectIterator.h"
#include "odblib/ReaderIterator.h"
#include "odblib/Comparator.h"
#include "odblib/odb_api.h"

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

