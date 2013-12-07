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

// TODO: (test not finished yet)


void UnitTest::setUp() { }

/// UnitTest syntax 'select lat, lon' (no file name)
///
void UnitTest::test()
{
	selectStarOneColumn();
	selectSumOneColumn();
}

void UnitTest::selectStarOneColumn()
{
	string sql = "select *";
	const string fileName = "2000010106.4.0.oda.csv";//"UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs, ",");
	
	Log::info(Here()) << "UnitTest::selectStarOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	Log::info() << "UnitTest::selectStarOneColumn: columns().size():  " << it->columns().size() << std::endl;

	unsigned long n = 0;
	for( ; it != end; ++it, ++n)
	{
	}

	Log::info() << "UnitTest::selectStarOneColumn: number of rows:  " << n << std::endl;
}

void UnitTest::selectSumOneColumn()
{
/*
	string sql = "select sum(a), sum(b)";
	const string fileName = "UnitTest.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs);
	
	Log::info(Here()) << "UnitTest::selectSumOneColumn: Execute '" << sql << "'" << std::endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	ASSERT((*it).columns().size() == 2);

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
	ASSERT((*it)[1] == 55);
*/
}


void UnitTest::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 



