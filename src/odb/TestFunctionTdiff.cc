/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionTdiff.cc
///
/// @author ECMWF, July 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#define RMDI   -2147483647
#define EPS    7e-6

using namespace std;

#include "Tool.h"
#include "TestCase.h"
#include "TestFunctionTdiff.h"
#include "ToolFactory.h"
#include "odblib/piconst.h"

#define SRC __FILE__, __LINE__

#include "odblib/oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionTdiff> _TestFunctionTdiff("TestFunctionTdiff");

TestFunctionTdiff::TestFunctionTdiff(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionTdiff::~TestFunctionTdiff() { }


void TestFunctionTdiff::test()
{
	testReaderIterator();
}

void TestFunctionTdiff::setUp()
{
	Timer t("Test tdiff function");
	odb::Writer<> oda("test_tdiff.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "date", odb::INTEGER);
	row->setColumn(1, "time", odb::INTEGER);
	
	row->writeHeader();

	(*row)[0] = 20090706.0;
	(*row)[1] = 210109.0;

    ++row;
}

void TestFunctionTdiff::tearDown() 
{ 
	ksh("rm -f test_tdiff.odb", SRC);
}

void TestFunctionTdiff::testReaderIterator()
{
    const string sql = "select tdiff(date,time,20090707.0,0.0) from \"test_tdiff.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT((*it)[0] == -10731); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

