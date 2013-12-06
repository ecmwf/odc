/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectIterator3.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/utils/Timer.h"
#include "odblib/ODBSelect.h"
#include "odblib/ToolFactory.h"
#include "odblib/Writer.h"
#include "odb/TestSelectIterator3.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSelectIterator3> _TestSelectIterator3("TestSelectIterator3");

TestSelectIterator3::TestSelectIterator3(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSelectIterator3::~TestSelectIterator3() { }

const string testFile = "TestSelectIterator3.odb";

const double VALUE[] = { 1, 2, 3 };

void TestSelectIterator3::setUp()
{
	Log::debug() << "TestSelectIterator3::setUp" << std::endl;

	Timer t("Writing " + testFile );
	odb::Writer<> oda(testFile);

	odb::Writer<>::iterator writer = oda.begin();
	writer->columns().setSize(1);
	(**writer).setColumn(0, "value", odb::INTEGER);
	(**writer).writeHeader();

	for (size_t i = 0; i < sizeof(VALUE) / sizeof(double); ++i)
	{
		(*writer)[0] = VALUE[i]; // col 0
		++writer;
	}
}

///
/// Tests problem fixed with p4 change 23687
///
void TestSelectIterator3::test()
{
	const string SELECT = "select * from \"" + testFile + "\";";

	odb::Select oda(SELECT);
	size_t i=0;
	for (odb::Select::iterator it = oda.begin();
		it != oda.end() && i < sizeof(VALUE) / sizeof(double);
		++it, ++i) 
	{
		Log::info() << "TestSelectIterator3::testBug01: it[" << i << "]=" << (*it)[0] << ", should be " << VALUE[i] << std::endl;
		ASSERT((*it)[0] == VALUE[i]);
	}
}

void TestSelectIterator3::tearDown() { }

} // namespace test
} // namespace tool 
} // namespace odb 

