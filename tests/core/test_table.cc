/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>

#include "eckit/testing/Test.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/Resource.h"

using namespace eckit::testing;
using eckit::Log;


// ------------------------------------------------------------------------------------------------------

eckit::Resource<eckit::PathName> testDataPath("$TEST_DATA_DIRECTORY", "..");

CASE("Table span") {

    ASSERT(false);

}


// ------------------------------------------------------------------------------------------------------

//static void test()
//{
////	const char * configFile = "/tmp/p4/mars/server/dev/oda/mars/marsKeywordToODBColumn";
//	const char * config = 
//	"DATE: andate\n"
//	"TIME: antime\n"
//	"REPORTYPE: reportype\n"
//	"CLASS: class\n"
//	"TYPE: type\n"
//	"STREAM: stream\n"
//	"OBSGROUP: groupid\n"
//	"EXPVER: expver\n"
//	;
//
//	FastODA2Request<ODA2RequestClientTraits> o;
//	//o.parseConfig(StringTool::readFile(cfgFile));
//	o.parseConfig(config);
//
//	OffsetList offsets;
//	LengthList lengths;
//	vector<ODAHandle*> handles;
//
//	//PathName pathName("mondb_conv.17.16001.odb.fn6x");
//	ASSERT(getenv("odc_TEST_DATA_PATH") && "odc_TEST_DATA_PATH must be set");
//	string e = Resource<std::string>("$odc_TEST_DATA_PATH", string("../../../odc/src/odb"));
//	PathName pathName(e + "/mondb.1.12.odb");
//	bool rc = o.scanFile(pathName, offsets, lengths, handles);
//	ASSERT(rc == true);
//
//	for (size_t i = 0; i < handles.size(); ++i)
//		Log::info() << "test: handles[" << i << "]=" << *handles[i] << std::endl;
//
//	string r = o.genRequest();
//	Log::info() << "test: o.genRequest() => " << std::endl << r << std::endl;
//
//	unsigned long long n = o.rowsNumber();
//	Log::info() << "test: rowsNumber == " << n <<  std::endl;
//}
//
//


// ------------------------------------------------------------------------------------------------------


//static Length size1;
//static Length size2;
//
//Length createFile(const std::string& fileName, unsigned int andate, unsigned int antime, unsigned int reportype)
//{
//	{
//		odc::Writer<> oda(fileName);
//		odc::Writer<>::iterator row = oda.begin();
//		row->setNumberOfColumns(3);
//        row->setColumn(0, "antime", odc::api::INTEGER);
//        row->setColumn(1, "andate", odc::api::INTEGER);
//        row->setColumn(2, "reportype", odc::api::INTEGER);
//		row->writeHeader();
//		for (size_t i = 0; i < 1; ++i, ++row)
//		{
//			(*row)[0] = andate; 
//			(*row)[1] = antime;
//			(*row)[2] = reportype;
//			++row;
//		}
//	}
//	PathName p(fileName);
//	return p.size();
//} 
//
//static void setUp()
//{
//
//    size1 = createFile("TestFastODA2Request21.odb", 20110823, 0, 21);
//    size2 = createFile("TestFastODA2Request22.odb", 20110823, 0, 22);
//
//    const char *cmd =
//    "cat "
//    "TestFastODA2Request21.odb TestFastODA2Request21.odb TestFastODA2Request21.odb "
//    "TestFastODA2Request22.odb TestFastODA2Request22.odb "
//    " >TestFastODA2Request2BIG.odb"
//    ;
//
//    int catStatus = system(cmd);
//    ASSERT(WEXITSTATUS(catStatus) == 0);
//
//    catStatus = system("cat TestFastODA2Request2BIG.odb TestFastODA2Request2BIG.odb >TestFastODA2Request2BAD.odb");
//    ASSERT(WEXITSTATUS(catStatus) == 0);
//}
//static void test()
//{
//	const char * config = "let, DATE = andate, TIME = antime, REPORTYPE = reportype" ;
//
//	FastODA2Request<ODA2RequestServerTraits> o;
//	//o.parseConfig(StringTool::readFile(cfgFile));
//	o.parseConfig(config);
//
//	OffsetList offsets;
//	LengthList lengths;
//	vector<ODAHandle*> handles;
//
//	o.scanFile("TestFastODA2Request2BIG.odb", offsets, lengths, handles);
//
//	for (size_t i = 0; i < handles.size(); ++i)
//		Log::info() << "test: handles[" << i << "]=" << *handles[i] << std::endl;
//
//	ASSERT(handles.size() == 2);
//	ASSERT(0 == handles[0]->start());
//    ASSERT(size1 * 3 == handles[0]->end());
//    ASSERT(size1 * 3 == handles[1]->start());
//    ASSERT(size1 * 3 + size2 * 2 == handles[1]->end());
//
//	string r = o.genRequest();
//	Log::info() << "test: o.genRequest() => " << std::endl << r << std::endl;
//
//	unsigned long long n = o.rowsNumber();
//	Log::info() << "test: rowsNumber == " << n <<  std::endl;
//	ASSERT(n == 2 * (2 + 3));
//
//	
//	OffsetList offsets2;
//	LengthList lengths2;
//	vector<ODAHandle*> handles2;
//
//	FastODA2Request<ODA2RequestClientTraits> o2;
//	o2.parseConfig(config);
//	bool rc = o2.scanFile("TestFastODA2Request2BAD.odb", offsets2, lengths2, handles2);
//	ASSERT(rc == false);
//
//	OffsetList offsets3;
//	LengthList lengths3;
//	vector<ODAHandle*> handles3;
//
//	FastODA2Request<ODA2RequestServerTraits> o3;
//	o3.parseConfig(config);
//	
//	bool exceptionThrown = false;
//	try { o3.scanFile("TestFastODA2Request2BAD.odb", offsets3, lengths3, handles3); }
//	catch (UserError e)
//	{
//		exceptionThrown = true;
//	}
//	ASSERT(exceptionThrown);
//}


// ------------------------------------------------------------------------------------------------------

//const char * cfg = "let, DATE = 'andate@desc', TIME = 'antime@desc', TYPE = type, EXPVER = expver";
//
//static void test()
//{
//    odc::FastODA2Request<odc::ODA2RequestClientTraits> o;
//	o.parseConfig(cfg);
//	o.scanFile("2000010106.2.0.odb");
//
//	string r = o.genRequest();
//	unsigned long long n = o.rowsNumber();
//
//	Log::info() << "test: request is:" << std::endl << r << std::endl;
//	Log::info() << "test: file has " << n << " rows(s)." << std::endl;
//
//	ASSERT(o.getValues("DATE").size() == 1);
//	ASSERT(*o.getValues("DATE").begin() == "20000101");
//
//	ASSERT(o.getValues("TIME").size() == 1);
//	ASSERT(*o.getValues("TIME").begin() == "60000");
//
//	ASSERT(o.getValues("TYPE").size() == 1);
//	ASSERT(*o.getValues("TYPE").begin() == "MISSING");
//
//	map<string, double> m = o.getUniqueValues();
//	ASSERT(m["TYPE"] == odc::MDI::realMDI());
//	ASSERT(m["DATE"] == 20000101);
//	ASSERT(m["TIME"] == 60000);
//}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

