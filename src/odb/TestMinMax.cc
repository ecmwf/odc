/// \file TestMinMax.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define __STDC_LIMIT_MACROS

#include <stdint.h>

# define INT32_MIN		(-2147483647-1)
# define INT32_MAX		(2147483647)

using namespace std;

#include "Tool.h"
#include "TestCase.h"
#include "TestMinMax.h"
#include "ToolFactory.h"

#define SRC __FILE__, __LINE__

#include "oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestMinMax> _TestMinMax("TestMinMax");

TestMinMax::TestMinMax(int argc, char **argv)
: TestCase(argc, argv)
{}

TestMinMax::~TestMinMax() { }


const string SELECT  = "select * from \"test.oda\";";

void TestMinMax::test()
{
	testReaderIterator();
}

void TestMinMax::setUp()
{
	Timer t("Writing testminmax.oda");
	odb::Writer<> oda("testminmax.oda");

	odb::Writer<>::iterator row = oda.begin();
	row->columns().setSize(2);

	row->setColumn(0, "intcol", odb::INTEGER);
	row->setColumn(1, "realcol", odb::REAL);
	
	row->writeHeader();

	(*row)[0] = INT32_MIN;
	(*row)[1] = 1;
	++row;

	(*row)[0] = INT32_MAX;
	(*row)[1] = 1;
	++row;
	
	(*row)[0] = INT32_MAX + INT32_MIN;
	(*row)[1] = 1;
	++row;
}

void TestMinMax::tearDown() { }

void TestMinMax::testReaderIterator()
{
	odb::Reader oda("testminmax.oda");
	odb::Reader::iterator it = oda.begin();

	//ASSERT(it->integer(0) == INT32_MIN);
	ASSERT((*it)[0] == INT32_MIN);
	++it;

	//ASSERT(it->integer(0) == INT32_MAX);
	ASSERT((*it)[0] == INT32_MAX);
	++it;

	//ASSERT(it->integer(0) == INT32_MAX + INT32_MIN);
	ASSERT((*it)[0] == INT32_MAX + INT32_MIN);
	++it;

	//ASSERT(! (it != oda.end()));
}

} // namespace test 
} // namespace tool 
} // namespace odb 

