/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.h
///
// @author Piotr Kuchta, ECMWF, Oct 2010

#include "odblib/MetaData.h"
#include "odblib/Select.h"

#include "UnitTest.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {


void UnitTest::setUp()
{
	ofstream f("UnitTest.txt");
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

/// UnitTest syntax 'select lat, lon' (no file name)
///
void UnitTest::test()
{
	selectStarOneColumn();
	selectSumOneColumn();
}

void UnitTest::selectStarOneColumn()
{
	string sql = "select * where a > 4;";
	const string fileName = "UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "UnitTest::selectStarOneColumn: Execute '" << sql << "'" << std::endl;
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

void UnitTest::selectSumOneColumn()
{
	string sql = "select sum(a), sum(b);";
	const string fileName = "UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "UnitTest::selectSumOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
	ASSERT((*it)[1] == 55);
}


void UnitTest::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 



