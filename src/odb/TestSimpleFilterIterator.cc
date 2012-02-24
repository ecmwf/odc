/// \file TestSimpleFilterIterator.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include <iostream>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestSimpleFilterIterator.h"
#include "ToolFactory.h"
#include "SimpleFilterIterator.h"
#include "SimpleFilter.h"
#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {
namespace test {

//int const OBSTYPE = 1;
//int const SENSOR = 24;

ToolFactory<TestSimpleFilterIterator> _TestSimpleFilterIterator("TestSimpleFilterIterator");

TestSimpleFilterIterator::TestSimpleFilterIterator(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSimpleFilterIterator::~TestSimpleFilterIterator() { }

/// Tests DispatchingWriter
///
void TestSimpleFilterIterator::test()
{
	const string fileName = "../odb2oda/2000010106/ECMA.odb";
	string sql = string("select * from \"") + fileName + "\" where obstype = 7;";
	long n0 = 0;
	long n1 = 0;
	long n2 = 0;

	{
		Timer t("TestSimpleFilterIterator::test: selecting rows where obstype == 7 the simple way");
		odb::Reader oda(fileName);
		odb::Reader::iterator it = oda.begin();
		size_t obstype_index = it->columns().columnIndex("obstype");
		for (; it != oda.end(); ++it)
		{
			if ((*it)[obstype_index] == 7.0)
			{
				++n0;
#if 0
				if (n0 < 10 || n0 > 3134386 - 3)
				{
					cerr << "" << n0 << ": "; 
					for (size_t i=30; i < 40; ++i)
						cerr << it[i] << ", ";
					cerr << endl;
				}
#endif
			}
		}
	}

	{
		Timer t("TestSimpleFilterIterator::test: selecting rows where obstype == 7");

		odb::Reader oda(fileName);

		typedef odb::SimpleFilter<odb::Reader::iterator> Filter;
		Filter simpleFilter(oda.begin(), oda.end(), "obstype", 7.0);
		Filter::iterator sIt = simpleFilter.begin();
		for (; sIt != simpleFilter.end(); ++sIt)
		{
			++n1;
#if 0
			if (n1 < 10 || n1 > 3134386 - 3)
			{
				cerr << "" << n1 << ": "; 
				for (size_t i=30; i < 40; ++i)
					cerr << sIt[i] << ", ";
				cerr << endl;
			}
#endif
		}

		Log::info() << "TestSimpleFilterIterator::test: selected " << n1 << " rows." << endl;
	}

	{
		Timer t("TestSimpleFilterIterator::test: selecting rows using SQL where obstype == 7");
		Log::info() << "TestSimpleFilterIterator::test: Execute '" << sql << "'" << endl;

		odb::Select odas(sql, fileName);
		for(odb::Select::iterator it = odas.begin(); it != odas.end(); ++it)
		{
			++n2;

#if 0
			if (n2 < 10 || n2 > 3134386 - 3)
			{
				cerr << "" << n2 << ": ";
				for (size_t i=30; i < 40; ++i)
					cerr << it[i] << ", ";
				cerr << endl;
			}
#endif
		}

		Log::info() << "TestSimpleFilterIterator::test: selected " << n2 << " rows." << endl;
	}

	Log::info() << "TestSimpleFilterIterator::test: n0=" << n0 << ", n1=" << n1 << ", n2=" << n2 << endl;
	ASSERT(n0 == n1);
	ASSERT(n1 == n2);
}

void TestSimpleFilterIterator::setUp() {}
void TestSimpleFilterIterator::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 

