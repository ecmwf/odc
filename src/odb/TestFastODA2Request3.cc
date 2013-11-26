/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFastODA2Request3.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

#include "odblib/odb_api.h"

#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/TestCase.h"
#include "odblib/FastODA2Request.h"
#include "TestFastODA2Request3.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

//ToolFactory<TestFastODA2Request3> _TestFastODA2Request3("TestFastODA2Request3");

TestFastODA2Request3::TestFastODA2Request3(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFastODA2Request3::~TestFastODA2Request3() {}

void TestFastODA2Request3::setUp() {}

void TestFastODA2Request3::test()
{
	const char * configFile = "/tmp/p4/mars/server/dev/oda/mars/marsKeywordToODBColumn";
	const char * config = 
	"DATE: andate\n"
	"TIME: antime\n"
	"REPORTYPE: reportype\n"
	"CLASS: class\n"
	"TYPE: type\n"
	"STREAM: stream\n"
	"OBSGROUP: groupid\n"
	"EXPVER: expver\n"
	;

	FastODA2Request<ODA2RequestClientTraits> o;
	//o.parseConfig(StringTool::readFile(cfgFile));
	o.parseConfig(config);

	OffsetList offsets;
	LengthList lengths;
	vector<ODAHandle*> handles;

	//PathName pathName("mondb_conv.17.16001.odb.fn6x");
	ASSERT(getenv("ODB_API_TEST_DATA_PATH") && "ODB_API_TEST_DATA_PATH must be set");
	string e(Resource<string>("$ODB_API_TEST_DATA_PATH", string("../../../odb_api/src/odb")));
	PathName pathName(e + "/mondb.1.12.odb");
	bool rc = o.scanFile(pathName, offsets, lengths, handles);
	ASSERT(rc == true);

	for (size_t i = 0; i < handles.size(); ++i)
		Log::info() << "TestFastODA2Request3::test: handles[" << i << "]=" << *handles[i] << std::endl;

	string r = o.genRequest();
	Log::info() << "TestFastODA2Request3::test: o.genRequest() => " << endl << r << std::endl;

	unsigned long long n = o.rowsNumber();
	Log::info() << "TestFastODA2Request3::test: rowsNumber == " << n <<  std::endl;
}


void TestFastODA2Request3::tearDown() { }

} // namespace test 
} // namespace tool 
} // namespace odb 

