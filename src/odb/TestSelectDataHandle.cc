/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectDataHandle.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2010

//#include <iostream>

//#include "eckit/io/FileHandle.h"
//#include "odblib/odb_api.h"
//#include "odblib/Tool.h"
//#include "odblib/TestCase.h"
//#include "odblib/ToolFactory.h"
//#include "odblib/SimpleFilterIterator.h"
//#include "odblib/TemporaryFile.h"
//#include "eckit/utils/Timer.h"
//#include "odblib/Writer.h"
//#include "odblib/ODBSelect.h"

//#include "odb/TestSelectDataHandle.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSelectDataHandle> _TestSelectDataHandle("TestSelectDataHandle");

TestSelectDataHandle::TestSelectDataHandle(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSelectDataHandle::~TestSelectDataHandle() { }

/// Tests syntax 'select lat, lon' (no file name)
///
void TestSelectDataHandle::test()
{
	string sql = "select * where obstype = 7;";
	//string sql = "select * where obstype = 7;";
	//string sql = "select obstype from \"input.oda\";";

	const string fileName = "2000010106.odb";
	FileHandle fh(fileName);
	fh.openForRead();
	odb::Select oda(sql, fh);
	
	Log::info(Here()) << "TestSelectDataHandle::test: Execute '" << sql << "'" << std::endl;
	long n = 0;
	{
		Timer t("TestSelectDataHandle::test: selecting rows using SQL" );

		odb::Select::iterator it = oda.begin();
		odb::Select::iterator end = oda.end();

		for( ; it != end; ++it)
			++n;
	}
	Log::info(Here()) << "TestSelectDataHandle::test: selected " << n << " rows." << std::endl;
	ASSERT(n == 3134386); 
	fh.close();
}

void TestSelectDataHandle::setUp()
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

void TestSelectDataHandle::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

