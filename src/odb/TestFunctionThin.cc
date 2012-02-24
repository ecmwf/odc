/// \file TestFunctionThin.cc
///
/// @author ECMWF, July 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#define RMDI -2147483647
#define EPS  7e-6

using namespace std;

#include "Tool.h"
#include "TestCase.h"
#include "TestFunctionThin.h"
#include "ToolFactory.h"
#include "piconst.h"

#define SRC __FILE__, __LINE__

#include "oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionThin> _TestFunctionThin("TestFunctionThin");

TestFunctionThin::TestFunctionThin(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionThin::~TestFunctionThin() {}

void TestFunctionThin::test()
{
	testReaderIterator();
}

void TestFunctionThin::setUp()
{
	Timer t("Test thin function");
	odb::Writer<> oda("test_thin.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(1);

	row->setColumn(0, "lat", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
	(*++row)[0] = 45.0;
}

void TestFunctionThin::tearDown() 
{ 
	ksh("rm -f test_thin.odb", SRC);
}

void TestFunctionThin::testReaderIterator()
{
    const string sql = "select thin(2.0,lat) from \"test_thin.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

    int i=0;
	odb::Select oda(sql);
    for(odb::Select::iterator it = oda.begin(); it != oda.end(); ++it) {
      if (i % 2 == 1) 
	    ASSERT(fabs((*it)[0] - 0.0e0) < EPS); // 
      else
	    ASSERT(fabs((*it)[0] - 1.0) < EPS); // 
      ++i;
    }

}

} // namespace test 
} // namespace tool 
} // namespace odb 

