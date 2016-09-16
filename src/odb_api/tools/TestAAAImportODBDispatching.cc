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

#include "eckit/log/Log.h"
#include "eckit/log/CodeLocation.h"

#include "odb_api/StringTool.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void test()
{
	if (getenv("ODB_ROOT"))
        odb::StringTool::shell("MALLOC_CHECK_=2 ODB_COMPILER_FLAGS=`pwd`/2000010106/ECMA/ECMA.flags odb_migrator 2000010106/ECMA . 2000010106.{obstype}.{sensor}.odb", Here());
	else {
		Log::warning() << "UnitTest: ODB_ROOT not set, skipping testing of odb_migrator" << std::endl;
        odb::StringTool::shell("odb split 2000010106.odb 2000010106.{obstype}.{sensor}.odb", Here());
	}
}
static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(AAAImportODBDispatching)
