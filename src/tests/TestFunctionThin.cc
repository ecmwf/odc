/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.cc
///
/// @author ECMWF, July 2010

const double EPS =   7e-6;

#include <cmath>

#include "eckit/log/Timer.h"
#include "odblib/Select.h"

#include "odblib/Writer.h"
#include "UnitTest.h"

using namespace std;
using namespace eckit;


namespace odb {
namespace tool {
namespace test {



(int argc, char **argv)
: UnitTest(argc, argv)
{}



void UnitTest::test()
{
	testReaderIterator();
}

void UnitTest::setUp()
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

void UnitTest::tearDown() 
{ 
	PathName("test_thin.odb").unlink();
}

void UnitTest::testReaderIterator()
{
    const string sql = "select thin(2.0,lat) from \"test_thin.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

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



