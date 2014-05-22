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

#include "odb_api/odblib/MetaData.h"
#include "odb_api/odblib/Select.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;


static void setUp()
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



static void selectStarOneColumn()
{
	string sql = "select * where a > 4;";
	const string fileName = "UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "selectStarOneColumn: Execute '" << sql << "'" << std::endl;
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

static void selectSumOneColumn()
{
	string sql = "select sum(a), sum(b);";
	const string fileName = "UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "selectSumOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
	ASSERT((*it)[1] == 55);
}


/// UnitTest syntax 'select lat, lon' (no file name)
///
static void test()
{
    selectStarOneColumn();
    selectSumOneColumn();
}


static void tearDown(){}

SIMPLE_TEST(TextSelect2)
