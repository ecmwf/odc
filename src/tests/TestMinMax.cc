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
#include "odblib/Reader.h"

#include "odblib/Writer.h"
#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

#ifndef INT32_MAX
# define INT32_MAX		(2147483647)
#endif

#ifndef INT32_MIN
# define INT32_MIN		(-INT32_MAX-1)
#endif

const string SELECT  = "select * from \"test.odb\";";


static void setUp()
{
	Timer t("Writing testminmax.odb");
	odb::Writer<> oda("testminmax.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->setNumberOfColumns(2);

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

static void tearDown() { }

static void test()
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



RUN_SIMPLE_TEST
