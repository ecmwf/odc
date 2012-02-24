/// \file TestSelectDataHandle.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2010

#include <iostream>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestSelectDataHandle.h"
#include "ToolFactory.h"
#include "SimpleFilterIterator.h"
#include "TmpFile.h"
#define SRC __FILE__, __LINE__

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

	const string fileName = "../odb2oda/2000010106/ECMA.odb";
	FileHandle fh(fileName);
	fh.openForRead();
	odb::Select oda(sql, fh);
	
	Log::info(SRC) << "TestSelectDataHandle::test: Execute '" << sql << "'" << endl;
	long n = 0;
	{
		Timer t("TestSelectDataHandle::test: selecting rows using SQL" );

		odb::Select::iterator it = oda.begin();
		odb::Select::iterator end = oda.end();

		for( ; it != end; ++it)
			++n;
	}
	Log::info(SRC) << "TestSelectDataHandle::test: selected " << n << " rows." << endl;
	ASSERT(n == 3134386); 
	fh.close();
}

void TestSelectDataHandle::setUp()
{
#if 0
	string s = "Data to be saved";
	
	TmpFile tmp;
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

