/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAggregateFunctions3.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include "odblib/ImportTool.h"
#include "odblib/ODBSelect.h"
#include "odblib/Reader.h"
#include "odblib/ToolFactory.h"
#include "odb/TestAggregateFunctions3.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestAggregateFunctions3> _TestAggregateFunctions3("TestAggregateFunctions3");

TestAggregateFunctions3::TestAggregateFunctions3(int argc, char **argv)
: TestCase(argc, argv)
{}

TestAggregateFunctions3::~TestAggregateFunctions3() { }

///
void TestAggregateFunctions3::test()
{
	string sql = "select sum(a) from \"TestAggregateFunctions3.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	ASSERT( (*it2)[0] == 55);
}

void TestAggregateFunctions3::setUp()
{
	stringstream s;
	s << "a:REAL" << std::endl;
	for (size_t i = 1; i <= 10; ++i)
		s << i << std::endl;
	ImportTool::importText(s.str().c_str(), "TestAggregateFunctions3.odb");
}

void TestAggregateFunctions3::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

