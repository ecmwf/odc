/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTextSelect2.h
///
// @author Piotr Kuchta, ECMWF, Oct 2010

#include "odblib/MetaData.h"
#include "odblib/Select.h"
#include "odblib/ToolFactory.h"
#include "TestTextSelect2.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestTextSelect2> _TestTextSelect2("TestTextSelect2");

TestTextSelect2::TestTextSelect2(int argc, char **argv)
: TestCase(argc, argv)
{}

TestTextSelect2::~TestTextSelect2() { }

void TestTextSelect2::setUp()
{
	ofstream f("TestTextSelect2.txt");
	f << 
	"a:REAL,b:REAL\n"
	"1,1\n"
	"2,2\n"
	"3,3\n"
	"4,4\n"
	"5,5\n"
	"6,6\n"
	"7,7\n"
	"8,8\n"
	"9,9\n"
	"10,10\n";
}

/// Tests syntax 'select lat, lon' (no file name)
///
void TestTextSelect2::test()
{
	selectStarOneColumn();
	selectSumOneColumn();
}

void TestTextSelect2::selectStarOneColumn()
{
	string sql = "select * where a > 4;";
	const string fileName = "TestTextSelect2.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "TestTextSelect2::selectStarOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	double v = 5;
	unsigned long n = 0;
	for( ; it != end; ++it, ++n)
	{
		ASSERT((*it).columns().size() == 2);
		ASSERT(v++ == (*it)[0]);
	}

	ASSERT(n == 6);
}

void TestTextSelect2::selectSumOneColumn()
{
	string sql = "select sum(a), sum(b);";
	const string fileName = "TestTextSelect2.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "TestTextSelect2::selectSumOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
	ASSERT((*it)[1] == 55);
}


void TestTextSelect2::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

