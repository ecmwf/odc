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

#include "odblib/FastODA2Request.h"
#include "odblib/ODAHandle.h"

#include "odblib/Writer.h"
#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

static Length size1;
static Length size2;

Length createFile(const std::string& fileName, unsigned int andate, unsigned int antime, unsigned int reportype)
{
	{
		odb::Writer<> oda(fileName);
		odb::Writer<>::iterator row = oda.begin();
		row->setNumberOfColumns(3);
		row->setColumn(0, "antime", odb::INTEGER);
		row->setColumn(1, "andate", odb::INTEGER);
		row->setColumn(2, "reportype", odb::INTEGER);
		row->writeHeader();
		for (size_t i = 0; i < 1; ++i, ++row)
		{
			(*row)[0] = andate; 
			(*row)[1] = antime;
			(*row)[2] = reportype;
			++row;
		}
	}
	PathName p(fileName);
	return p.size();
} 

static void setUp()
{
    size1 = createFile("UnitTest.odb", 20110823, 0, 21);
    size2 = createFile("UnitTest.odb", 20110823, 0, 22);

	const char *cmd =
	"cat "
	"UnitTest.odb UnitTest.odb UnitTest.odb "
	"UnitTest.odb UnitTest.odb "
	" >UnitTest.odb"
	;

	int catStatus = system(cmd);
	ASSERT(WEXITSTATUS(catStatus) == 0);

	catStatus = system("cat UnitTest.odb UnitTest.odb >UnitTest.odb");
	ASSERT(WEXITSTATUS(catStatus) == 0);
}
static void test()
{
	const char * configFile = "/tmp/p4/mars/server/dev/oda/mars/marsKeywordToODBColumn";
	const char * config = 
	"DATE: andate\n"
	"TIME: antime\n"
	"REPORTYPE: reportype\n"
	;

	FastODA2Request<ODA2RequestServerTraits> o;
	//o.parseConfig(StringTool::readFile(cfgFile));
	o.parseConfig(config);

	OffsetList offsets;
	LengthList lengths;
	vector<ODAHandle*> handles;

	o.scanFile("UnitTest.odb", offsets, lengths, handles);

	for (size_t i = 0; i < handles.size(); ++i)
		Log::info() << "test: handles[" << i << "]=" << *handles[i] << std::endl;

	ASSERT(handles.size() == 2);
	ASSERT(0 == handles[0]->start());
    ASSERT(size1 * 3 == handles[0]->end());
    ASSERT(size1 * 3 == handles[1]->start());
    ASSERT(size1 * 3 + size2 * 2 == handles[1]->end());

	string r = o.genRequest();
	Log::info() << "test: o.genRequest() => " << std::endl << r << std::endl;

	unsigned long long n = o.rowsNumber();
	Log::info() << "test: rowsNumber == " << n <<  std::endl;
	ASSERT(n == 2 * (2 + 3));

	
	OffsetList offsets2;
	LengthList lengths2;
	vector<ODAHandle*> handles2;

	FastODA2Request<ODA2RequestClientTraits> o2;
	o2.parseConfig(config);
	bool rc = o2.scanFile("UnitTest.odb", offsets2, lengths2, handles2);
	ASSERT(rc == false);

	OffsetList offsets3;
	LengthList lengths3;
	vector<ODAHandle*> handles3;

	FastODA2Request<ODA2RequestServerTraits> o3;
	o3.parseConfig(config);
	
	bool exceptionThrown = false;
	try { o3.scanFile("UnitTest.odb", offsets3, lengths3, handles3); }
	catch (UserError e)
	{
		exceptionThrown = true;
	}
	ASSERT(exceptionThrown);
}


static void tearDown() { }


RUN_SIMPLE_TEST
