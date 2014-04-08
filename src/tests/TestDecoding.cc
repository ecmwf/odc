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
/// @author Piotr Kuchta, ECMWF, June 2009

#include "eckit/log/Timer.h"
#include "odblib/Reader.h"

#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

/// UnitTest DispatchingWriter
///
static void test()
{
	const string fileName = "2000010106.odb";

	Timer t(std::string("test: reading file '") + fileName + "'");

	odb::Reader f(fileName);
	odb::Reader::iterator it = f.begin();
	odb::Reader::iterator end = f.end();

	unsigned long long n = 0;	
	for (; it != end; ++it)
		++n;

	Log::info() << "test decoded " << n << " lines." << std::endl;
}

static void setUp(){}
static void tearDown(){}

RUN_SIMPLE_TEST
