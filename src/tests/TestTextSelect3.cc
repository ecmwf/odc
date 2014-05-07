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

#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

// TODO: (test not finished yet)


static void setUp() { }


static void selectStarOneColumn()
{
	string sql = "select *";
	const string fileName = "2000010106.4.0.oda.csv";//"UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "selectStarOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	Log::info() << "selectStarOneColumn: columns().size():  " << it->columns().size() << std::endl;

	unsigned long n = 0;
	for( ; it != end; ++it, ++n)
	{
	}

	Log::info() << "selectStarOneColumn: number of rows:  " << n << std::endl;
}

static void selectSumOneColumn()
{
/*
	string sql = "select sum(a), sum(b)";
	const string fileName = "UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs);
	
	Log::info(Here()) << "selectSumOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
	ASSERT((*it)[1] == 55);
*/
}


/// UnitTest syntax 'select lat, lon' (no file name)
///
static void test()
{
    selectStarOneColumn();
    selectSumOneColumn();
}


static void tearDown(){}

RUN_SIMPLE_TEST
