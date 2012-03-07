/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTextSelect.h
///
// @author Piotr Kuchta, ECMWF, Oct 2010

#include <iostream>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestTextSelect.h"
#include "ToolFactory.h"
#include "SimpleFilterIterator.h"
#include "TmpFile.h"
#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestTextSelect> _TestTextSelect("TestTextSelect");

TestTextSelect::TestTextSelect(int argc, char **argv)
: TestCase(argc, argv)
{}

TestTextSelect::~TestTextSelect() { }

void TestTextSelect::setUp() { }

/// Tests syntax 'select lat, lon' (no file name)
///
void TestTextSelect::test()
{
	selectStarOneColumn();
	selectSumOneColumn();
}

void TestTextSelect::selectStarOneColumn()
{
	string sql = "select * where a > 4;";
	const string fileName = "TestTextSelect.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs);
	
	Log::info(SRC) << "TestTextSelect::selectStarOneColumn: Execute '" << sql << "'" << endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();


	double v = 5;
	unsigned long n = 0;
	for( ; it != end; ++it, ++n)
		ASSERT(v++ == (*it)[0]);

	ASSERT(n == 6);
}

void TestTextSelect::selectSumOneColumn()
{
	string sql = "select sum(a)";
	const string fileName = "TestTextSelect.txt";
	ifstream fs(fileName.c_str());
	
	odb::Select oda(sql, fs);
	
	Log::info(SRC) << "TestTextSelect::selectSumOneColumn: Execute '" << sql << "'" << endl;
	odb::Select::iterator it = oda.begin();
	odb::Select::iterator end = oda.end();

	++it;
	ASSERT(! (it != end));
	ASSERT((*it)[0] == 55);
}


void TestTextSelect::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

