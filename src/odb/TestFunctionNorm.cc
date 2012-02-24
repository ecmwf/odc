/// \file TestFunctionNorm.cc
///
/// @author ECMWF, July 2010

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#define RMDI   -2147483647
#define EPS    4e-5

using namespace std;

#include "Tool.h"
#include "TestCase.h"
#include "TestFunctionNorm.h"
#include "ToolFactory.h"
#include "piconst.h"

#define SRC __FILE__, __LINE__

#include "oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestFunctionNorm> _TestFunctionNorm("TestFunctionNorm");

TestFunctionNorm::TestFunctionNorm(int argc, char **argv)
: TestCase(argc, argv)
{}

TestFunctionNorm::~TestFunctionNorm() { }


void TestFunctionNorm::test()
{
	testReaderIterator();
}

void TestFunctionNorm::setUp()
{
	Timer t("Test Norm function");
	odb::Writer<> oda("test_norm.odb");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "x", odb::REAL);
	row->setColumn(1, "y", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = 3.0;
	(*row)[1] = 16.0;
    ++row;
	(*row)[0] = 4.0;
	(*row)[1] = 12.0;
    ++row;
	(*row)[0] = 2.0;
	(*row)[1] = 24.0;
    ++row;
}

void TestFunctionNorm::tearDown() 
{ 
	ksh("rm -f test_norm.odb", SRC);
}

void TestFunctionNorm::testReaderIterator()
{
    const string sql = "select norm(x,y) from \"test_norm.odb\";";

	Log::info() << "Executing: '" << sql << "'" << endl;

	odb::Select oda(sql);
	odb::Select::iterator it = oda.begin();

	ASSERT(fabs((*it)[0] - 12e0)<EPS); // 

}

} // namespace test 
} // namespace tool 
} // namespace odb 

