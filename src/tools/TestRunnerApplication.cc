/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <sstream>

#include "eckit/runtime/Context.h"

#include "Tool.h"
#include "TestCase.h"
#include "ToolFactory.h"
#include "TestRunnerApplication.h"
#include "TestRunner.h"
#include "odblib/ODBBehavior.h"

namespace odb {
namespace tool {
namespace test {

TestRunnerApplication::TestRunnerApplication (int argc, char **argv)
: ODBApplication(argc, argv)
{}

TestRunnerApplication::~TestRunnerApplication () {}

void TestRunnerApplication::run()
{
	TestRunner testRunner(commandLineParser());
	testRunner.run();
}

} // namespace test
} // namespace tool 
} // namespace odb 

