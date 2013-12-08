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

#include "eckit/config/Resource.h"
#include "eckit/types/Types.h"
#include "odblib/FastODA2Request.h"
#include "odblib/ODAHandle.h"
#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void test()
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
		Log::info() << "test: handles[" << i << "]=" << *handles[i] << std::endl;

	string r = o.genRequest();
	Log::info() << "test: o.genRequest() => " << endl << r << std::endl;

	unsigned long long n = o.rowsNumber();
	Log::info() << "test: rowsNumber == " << n <<  std::endl;
}


static void setUp(){}
static void tearDown(){}

RUN_SIMPLE_TEST
