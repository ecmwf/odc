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
#include "odblib/Select.h"

#include "UnitTest.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {



/// UnitTest syntax 'select lat, lon' (no file name)
///
void UnitTest::test()
{
	string sql = "select * where obstype = 7;";
	//string sql = "select * where obstype = 7;";
	//string sql = "select obstype from \"input.oda\";";

	const string fileName = "2000010106.odb";
	FileHandle fh(fileName);
	fh.openForRead();
	odb::Select oda(sql, fh);
	
	Log::info(Here()) << "UnitTest::test: Execute '" << sql << "'" << std::endl;
	long n = 0;
	{
		Timer t("UnitTest::test: selecting rows using SQL" );

		odb::Select::iterator it = oda.begin();
		odb::Select::iterator end = oda.end();

		for( ; it != end; ++it)
			++n;
	}
	Log::info(Here()) << "UnitTest::test: selected " << n << " rows." << std::endl;
	ASSERT(n == 3134386); 
	fh.close();
}

void UnitTest::setUp()
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

void UnitTest::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 



