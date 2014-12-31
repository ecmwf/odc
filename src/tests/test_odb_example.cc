/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#define BOOST_TEST_MODULE test_odb_example

#include "ecbuild/boost_test_framework.h"

#include "eckit/log/Bytes.h"
#include "eckit/config/Resource.h"
#include "eckit/types/Types.h"
#include "eckit/runtime/Tool.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace odb_test {

struct TestFixture : public eckit::Tool {
public:

	TestFixture() : Tool( boost::unit_test::framework::master_test_suite().argc,
						  boost::unit_test::framework::master_test_suite().argv )
	{
	}

	virtual void run() {} // we don't use this ...
};

}

//-----------------------------------------------------------------------------

using namespace odb_test;

BOOST_GLOBAL_FIXTURE( TestFixture );

BOOST_AUTO_TEST_SUITE( test_odb_someclass )

BOOST_AUTO_TEST_CASE( test_odb_someclass_constructor )
{
    string s = Resource<string>("s","some");

    BOOST_CHECK( s == "some" );

    double d = Resource<double>("d", 777.7);

	BOOST_CHECK_CLOSE( d , 777.7, 0.0001 ); // accept 0.0001% tolerance
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_odb_someclass_foobar )
{
	int myint = Resource<int>("integer;-integer",0);

	BOOST_CHECK_EQUAL( myint , 100 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

