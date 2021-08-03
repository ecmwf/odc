/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/log/Log.h"
#include "odc/core/MetaData.h"
#include "odc/Select.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

/// UnitTest problem fixed with p4 change 23687
///
static void test()
{
	string statusFields =
"status.active@body,status.passive@body,status.rejected@body,status.blacklisted@body,status.monthly@body,status.constant@body,status.experimental@body,status.whitelist@body";

	statusFields = "status.*@body";
	const string SELECT = std::string("select status@body, ") + statusFields + " from \"2000010106-reduced.odb\";";

	Log::info() << "Executing '" << SELECT << "'" << std::endl;

	odc::Select oda(SELECT);
	long int i=0;

	odc::Select::iterator it = oda.begin();

	ASSERT(it->columns().size() == 9);

	for ( ; it != oda.end() && i < 5000; ++it, ++i) 
	{
		unsigned int sum =
			  int ((*it)[1])
			| int ((*it)[2]) << 1
			| int ((*it)[3]) << 2
			| int ((*it)[4]) << 3
			| int ((*it)[5]) << 4
			| int ((*it)[6]) << 5
			| int ((*it)[7]) << 6
			| int ((*it)[8]) << 7;
//        Log::info() << i << ": " << (*it)[0] << " " << sum << std::endl;
		ASSERT((*it)[0] == sum);
	}
}



static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(Bitfields)
