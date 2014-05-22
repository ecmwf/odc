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
/// @author Piotr Kuchta, ECMWF, Jan 2011

#include "tools/ImportTool.h"
#include "odb_api/MetaData.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void setUp()
{
    odb::tool::ImportTool::importText("x:REAL\n1\n2\n3\n", "UnitTest.odb");
}


static void test()
{
	const char *fileName = "UnitTest.odb";

	MetaData wholeFileMD(MetaData::scanFile(fileName));

	ASSERT(wholeFileMD[0]->min() == 1 && wholeFileMD[0]->max() == 3);

	Log::info() << "test wholeFileMD==" << wholeFileMD << std::endl;
}

static void tearDown() { }


SIMPLE_TEST(MetaDataReader)
