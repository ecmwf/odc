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
/// @author Piotr Kuchta, ECMWF, Jan 2010

#include "eckit/io/FileHandle.h"
#include "eckit/log/Timer.h"
#include "odc/Select.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

/// UnitTest syntax 'select lat, lon' (no file name)
///
static void test()
{
	string sql = "select * where obstype = 7;";
	//string sql = "select * where obstype = 7;";
	//string sql = "select obstype from \"input.oda\";";

	const string fileName = "2000010106-reduced.odb";
	FileHandle fh(fileName);
	fh.openForRead();
	AutoClose closer(fh);
	odc::Select oda(sql, fh);
	
	Log::info() << "test: Execute '" << sql << "'" << std::endl;
	long n = 0;
	{
		Timer t("test: selecting rows using SQL" );

		odc::Select::iterator it = oda.begin();
		odc::Select::iterator end = oda.end();

		for( ; it != end; ++it)
			++n;
	}
	Log::info() << "test: selected " << n << " rows." << std::endl;
	ASSERT(n == 44969);
	fh.close();
}

static void setUp()
{
#if 0
	string s = "Data to be saved";
	
	TemporaryFile tmp;
	ofstream os(tmp.c_str());
	os << s;
	os.close();
	if(!os) throw WriteError(tmp);

	string cmd = "ls -l ";
	cmd += tmp;
	system(cmd.c_str());

	cmd = "cat ";
	cmd += tmp;
	system(cmd.c_str());
#endif
}


static void tearDown(){}

SIMPLE_TEST(SelectDataHandle)
