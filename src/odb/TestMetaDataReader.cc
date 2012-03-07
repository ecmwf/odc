/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestMetaDataReader.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

using namespace std;

#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "TestCase.h"

#include "TestMetaDataReader.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestMetaDataReader> _TestMetaDataReader("TestMetaDataReader");

TestMetaDataReader::TestMetaDataReader(int argc, char **argv)
: TestCase(argc, argv)
{}

TestMetaDataReader::~TestMetaDataReader() { }

void TestMetaDataReader::setUp() {}


void TestMetaDataReader::test()
{
	const char *fileName = "TestMetaDataReader.odb";

	MetaData wholeFileMD(MetaData::scanFile(fileName));

	ASSERT(wholeFileMD[0]->min() == 1 && wholeFileMD[0]->max() == 3);

	Log::info() << "TestMetaDataReader::test wholeFileMD==" << wholeFileMD << endl;
}

void TestMetaDataReader::tearDown() { }

} // namespace test 
} // namespace tool 
} // namespace odb 

