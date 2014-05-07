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
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/log/Timer.h"
#include "odb_api/odblib/Select.h"

#include "odb_api/odblib/Writer.h"
#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;


const string testFile = "UnitTest.odb";

const double VALUE[] = { 1, 2, 3 };

static void setUp()
{
	Log::debug() << "setUp" << std::endl;

	Timer t("Writing " + testFile );
	odb::Writer<> oda(testFile);

	odb::Writer<>::iterator writer = oda.begin();
    writer->setNumberOfColumns(1);
	(**writer).setColumn(0, "value", odb::INTEGER);
	(**writer).writeHeader();

	for (size_t i = 0; i < sizeof(VALUE) / sizeof(double); ++i)
	{
		(*writer)[0] = VALUE[i]; // col 0
		++writer;
	}
}

///
/// UnitTest problem fixed with p4 change 23687
///
static void test()
{
	const string SELECT = "select * from \"" + testFile + "\";";

	odb::Select oda(SELECT);
	size_t i=0;
	for (odb::Select::iterator it = oda.begin();
		it != oda.end() && i < sizeof(VALUE) / sizeof(double);
		++it, ++i) 
	{
		Log::info() << "testBug01: it[" << i << "]=" << (*it)[0] << ", should be " << VALUE[i] << std::endl;
		ASSERT((*it)[0] == VALUE[i]);
	}
}

static void tearDown() { }


RUN_SIMPLE_TEST
