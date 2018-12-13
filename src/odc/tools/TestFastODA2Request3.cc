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
#include "odc/FastODA2Request.h"
#include "odc/ODAHandle.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

static void test()
{
//	const char * configFile = "/tmp/p4/mars/server/dev/oda/mars/marsKeywordToODBColumn";
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
	ASSERT(getenv("odc_TEST_DATA_PATH") && "odc_TEST_DATA_PATH must be set");
	string e = Resource<std::string>("$odc_TEST_DATA_PATH", string("../../../odc/src/odb"));
	PathName pathName(e + "/mondb.1.12.odb");
	bool rc = o.scanFile(pathName, offsets, lengths, handles);
	ASSERT(rc == true);

	for (size_t i = 0; i < handles.size(); ++i)
		Log::info() << "test: handles[" << i << "]=" << *handles[i] << std::endl;

	string r = o.genRequest();
	Log::info() << "test: o.genRequest() => " << std::endl << r << std::endl;

	unsigned long long n = o.rowsNumber();
	Log::info() << "test: rowsNumber == " << n <<  std::endl;
}


static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(FastODA2Request3)
