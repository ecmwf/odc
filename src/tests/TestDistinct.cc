/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestDistinct.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include "eckit/filesystem/PathName.h"
#include "odblib/ImportTool.h"
#include "odblib/Select.h"
#include "odblib/ToolFactory.h"
#include "odb/TestDistinct.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestDistinct> _TestDistinct("TestDistinct");

TestDistinct::TestDistinct(int argc, char **argv)
: TestCase(argc, argv)
{}

TestDistinct::~TestDistinct() { }

///
void TestDistinct::test()
{
    std::string sql = "select distinct a from \"a1to10twice.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odb::Select sel(sql);
	odb::Select::iterator it2 = sel.begin();
	odb::Select::iterator end2 = sel.end();

	int i = 0;
	for (; it2 != end2; ++it2)
		ASSERT((*it2)[0] == ++i);

	ASSERT((*it2)[0] == 10);
}

void TestDistinct::setUp()
{
	stringstream s;
	s << "a:REAL" << std::endl;
	for (size_t i = 1; i <= 10; ++i) s << i << std::endl;
	for (size_t i = 1; i <= 10; ++i) s << i << std::endl;
	ImportTool::importText(s.str().c_str(), "a1to10twice.odb");
}

void TestDistinct::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

