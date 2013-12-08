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


#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;

using namespace std;
using namespace eckit;
//using namespace odb;



static void test()
{
	Log::info() << "I'm just a template, I don't test anything, really." << std::endl;
}

static void tearDown() { }


static void setUp(){}

RUN_SIMPLE_TEST
