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

#include "odb_api/Select.h"

#include "odb_api/Tracer.h"
#include "odb_api/Writer.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void setUp()
{
	Tracer t(Log::debug(), "setUp");
	{
		odb::Writer<> f("TestSelectTwoFiles1.odb");
		odb::Writer<>::iterator it = f.begin();
        it->setNumberOfColumns(1);
		it->setColumn(0, "a", odb::REAL);
		it->writeHeader();
		(*it)[0] = 1;
		++it;
	}
	{
		odb::Writer<> f("TestSelectTwoFiles2.odb");
		odb::Writer<>::iterator it = f.begin();
        it->setNumberOfColumns(1);
		it->setColumn(0, "b", odb::REAL);
		it->writeHeader();
		(*it)[0] = 2;
		++it;
	}
}

static void test()
{
	Tracer t(Log::debug(), "test");

    odb::Select s("select * from \"TestSelectTwoFiles1.odb\", \"TestSelectTwoFiles2.odb\";");
	//odb::Select s("select a,b from \"UnitTest.odb\", \"UnitTest.odb\"");
	odb::Select::iterator it = s.begin();
	odb::Select::iterator end = s.end();

    ASSERT(it->columns().size() == 2);

	unsigned long i = 0;
	for (; it != end; ++it)
	{
		Log::debug() << "test:    " << (*it)[0] << "    " << (*it)[0] << std::endl;

		ASSERT( ((*it)[0] == 1) && ((*it)[1] == 2) );
		++i;
	}

	ASSERT(i == 1);
}

static void tearDown() { }

SIMPLE_TEST(SelectTwoFiles)
