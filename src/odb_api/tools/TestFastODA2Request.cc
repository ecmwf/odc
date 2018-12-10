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
/// @author Piotr Kuchta, ECMWF, Jan 2011

#include "odb_api/FastODA2Request.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

const char * cfg = "let, DATE = 'andate@desc', TIME = 'antime@desc', TYPE = type, EXPVER = expver";

static void test()
{
    odc::FastODA2Request<odc::ODA2RequestClientTraits> o;
	o.parseConfig(cfg);
	o.scanFile("2000010106.2.0.odb");

	string r = o.genRequest();
	unsigned long long n = o.rowsNumber();

	Log::info() << "test: request is:" << std::endl << r << std::endl;
	Log::info() << "test: file has " << n << " rows(s)." << std::endl;

	ASSERT(o.getValues("DATE").size() == 1);
	ASSERT(*o.getValues("DATE").begin() == "20000101");

	ASSERT(o.getValues("TIME").size() == 1);
	ASSERT(*o.getValues("TIME").begin() == "60000");

	ASSERT(o.getValues("TYPE").size() == 1);
	ASSERT(*o.getValues("TYPE").begin() == "MISSING");

	map<string, double> m = o.getUniqueValues();
	ASSERT(m["TYPE"] == odc::MDI::realMDI());
	ASSERT(m["DATE"] == 20000101);
	ASSERT(m["TIME"] == 60000);
}


static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(FastODA2Request)
