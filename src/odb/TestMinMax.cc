/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestMinMax.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>


#ifndef INT32_MAX
# define INT32_MAX		(2147483647)
#endif

#ifndef INT32_MIN
# define INT32_MIN		(-INT32_MAX-1)
#endif

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "TestMinMax.h"
#include "odblib/ToolFactory.h"

#include "odblib/oda.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestMinMax> _TestMinMax("TestMinMax");

TestMinMax::TestMinMax(int argc, char **argv)
: TestCase(argc, argv)
{}

TestMinMax::~TestMinMax() { }


const string SELECT  = "select * from \"test.odb\";";

void TestMinMax::test()
{
	testReaderIterator();
}

void TestMinMax::setUp()
{
	Timer t("Writing testminmax.odb");
	odb::Writer<> oda("testminmax.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "intcol", odb::INTEGER);
	row->setColumn(1, "realcol", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = INT32_MIN;
	(*row)[1] = 1;
	++row;

	(*row)[0] = INT32_MAX;
	(*row)[1] = 1;
	++row;
	
	(*row)[0] = INT32_MAX + INT32_MIN;
	(*row)[1] = 1;
	++row;
}

void TestMinMax::tearDown() { }

void TestMinMax::testReaderIterator()
{
	odb::Reader oda("testminmax.odb");
	odb::Reader::iterator it = oda.begin();

	//ASSERT(it->integer(0) == INT32_MIN);
	ASSERT((*it)[0] == INT32_MIN);
	++it;

	//ASSERT(it->integer(0) == INT32_MAX);
	ASSERT((*it)[0] == INT32_MAX);
	++it;

	//ASSERT(it->integer(0) == INT32_MAX + INT32_MIN);
	ASSERT((*it)[0] == INT32_MAX + INT32_MIN);
	++it;

	//ASSERT(! (it != oda.end()));
}

} // namespace test 
} // namespace tool 
} // namespace odb 

