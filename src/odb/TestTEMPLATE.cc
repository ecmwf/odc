/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTEMPLATE.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

using namespace std;

#include "odblib/oda.h"

#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/TestCase.h"

#include "TestTEMPLATE.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestTEMPLATE> _TestTEMPLATE("TestTEMPLATE");

TestTEMPLATE::TestTEMPLATE(int argc, char **argv)
: TestCase(argc, argv)
{}

TestTEMPLATE::~TestTEMPLATE() { }

void TestTEMPLATE::setUp() {}


void TestTEMPLATE::test()
{
	Log::info() << "I'm just a template, I don't test anything, really." << endl;
}

void TestTEMPLATE::tearDown() { }

} // namespace test 
} // namespace tool 
} // namespace odb 

