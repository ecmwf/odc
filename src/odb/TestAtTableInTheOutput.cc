/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAtTableInTheOutput.h
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
#include "TestAtTableInTheOutput.h"
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
#include "Comparator.h"
#include "Tracer.h"
#include "SQLBitfield.h"

namespace odb {
namespace tool {
namespace test {


ToolFactory<TestAtTableInTheOutput> testTestAtTableInTheOutput("TestAtTableInTheOutput");

TestAtTableInTheOutput::TestAtTableInTheOutput(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAtTableInTheOutput::~TestAtTableInTheOutput() { }

void TestAtTableInTheOutput::test()
{
	selectIntoSecondFile();
	compareFiles();
}

void TestAtTableInTheOutput::setUp()
{
	Tracer t(Log::debug(), "TestAtTableInTheOutput::setUp");

	odb::Writer<> f("TestAtTableInTheOutput.odb");
	odb::Writer<>::iterator it = f.begin();
	MetaData& md = it->columns();

	md.setSize(4);
	it->setColumn(0, "lat@hdr", odb::REAL);
	it->setColumn(1, "lon@hdr", odb::REAL);
	it->setColumn(2, "obsvalue", odb::REAL);

	BitfieldDef bfDef;
	bfDef.first.push_back("x");
	bfDef.second.push_back(1);
	bfDef.first.push_back("y");
	bfDef.second.push_back(2);

	it->setBitfieldColumn(3, "bf", odb::BITFIELD, bfDef);

	it->writeHeader();

	for (size_t i = 1; i <= 2; i++)
	{
		(*it)[0] = i; // col 0
		(*it)[1] = i; // col 1
		(*it)[2] = i; // col 2
		++it;
	}
}

void TestAtTableInTheOutput::selectIntoSecondFile()
{
	Tracer t(Log::debug(), "TestAtTableInTheOutput::selectIntoSecondFile");

	const string fileName = "TestAtTableInTheOutput.odb";
	string sql = "select lat,lon,obsvalue,bf into \"TestAtTableInTheOutput2.odb\"";
	sql += " from \"" + fileName + "\" ;";

	odb::Select f(sql); //, fileName);
	odb::Select::iterator it = f.begin();

	//string c0 = it.columns()[0]->name();
	//string c1 = it.columns()[1]->name();
	//string c2 = it.columns()[2]->name();

	++it; // this is needed to push the second row to the INTO file 

	//Log::info()  << "c0=" << c0 << ", c1=" << c1 << ", c2=" << c2 << endl;
	///ASSERT("");
}

void TestAtTableInTheOutput::compareFiles()
{
	Tracer t(Log::debug(), "TestAtTableInTheOutput::compareFiles");

	odb::Reader oda1("TestAtTableInTheOutput.odb");
	odb::Reader oda2("TestAtTableInTheOutput2.odb");

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());
	
	odb::Comparator().compare(it1, end1, it2, end2, "TestAtTableInTheOutput.odb", "TestAtTableInTheOutput2.odb");
}

void TestAtTableInTheOutput::tearDown() { }


} // namespace test 
} // namespace tool 
} // namespace odb 

