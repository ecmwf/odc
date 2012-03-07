/*
 * © Copyright 1996-2012 ECMWF.
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

#include <Application.h>
#include <PathName.h>
#include <FileHandle.h>
#include <Timer.h>
#include <Tokenizer.h>

#include <sstream>

#include "Tool.h"
#include "TestCase.h"
#include "ToolFactory.h"
#include "TestRunnerApplication.h"
#include "TestRunner.h"

namespace odb {
namespace tool {
namespace test {

TestRunnerApplication::TestRunnerApplication (int argc, char **argv)
: Application(argc, argv),
  argc_(argc),
  argv_(argv)
{}

TestRunnerApplication::~TestRunnerApplication ()
{
	Log::info() << "TestRunnerApplication::~TestRunnerApplication" << endl;
}

void TestRunnerApplication::run()
{
	TestRunner testRunner(argc_, argv_);
	testRunner.run();
}

} // namespace test
} // namespace tool 
} // namespace odb 

