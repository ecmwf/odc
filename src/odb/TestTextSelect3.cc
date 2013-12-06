/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTextSelect3.h
///
// @author Piotr Kuchta, ECMWF, Oct 2010

//#include <iostream>

//#include "odblib/odb_api.h"
//#include "odblib/Tool.h"
//#include "odblib/TestCase.h"
#include "odb/TestTextSelect3.h"
#include "odblib/ToolFactory.h"
//#include "odblib/SimpleFilterIterator.h"
//#include "eckit/filesystem/TmpFile.h"
//#include "eckit/utils/Timer.h"
//#include "odblib/Writer.h"
#include "odblib/ODBSelect.h"
#include "odblib/MetaData.h"

//#include "odblib/Reader.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

// TODO: (test not finished yet)
//ToolFactory<TestTextSelect3> _TestTextSelect3("TestTextSelect3");

TestTextSelect3::TestTextSelect3(int argc, char **argv)
: TestCase(argc, argv)
{}

TestTextSelect3::~TestTextSelect3() { }

void TestTextSelect3::setUp() { }

/// Tests syntax 'select lat, lon' (no file name)
///
void TestTextSelect3::test()
{
	selectStarOneColumn();
	selectSumOneColumn();
}

void TestTextSelect3::selectStarOneColumn()
{
	string sql = "select *";
	const string fileName = "2000010106.4.0.oda.csv";//"TestTextSelect3.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "TestTextSelect3::selectStarOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	Log::info() << "TestTextSelect3::selectStarOneColumn: columns().size():  " << it->columns().size() << std::endl;

	unsigned long n = 0;
	for( ; it != end; ++it, ++n)
	{
	}

	Log::info() << "TestTextSelect3::selectStarOneColumn: number of rows:  " << n << std::endl;
}

void TestTextSelect3::selectSumOneColumn()
{
/*
	string sql = "select sum(a), sum(b)";
	const string fileName = "TestTextSelect3.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs);
	
	Log::info(Here()) << "TestTextSelect3::selectSumOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
	ASSERT((*it)[1] == 55);
*/
}


void TestTextSelect3::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

