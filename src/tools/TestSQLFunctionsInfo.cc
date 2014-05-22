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

#include "odb_api/odblib/FunctionFactory.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;


static void test()
{
	typedef odb::sql::expression::function::FunctionFactory::FunctionInfo FI;

	FI& functionsInfo = odb::sql::expression::function::FunctionFactory::instance().functionsInfo();

	Log::info() << "FunctionFactory::functionsInfo().size() == " << functionsInfo.size() << std::endl;
	for (FI::iterator i = functionsInfo.begin(); i != functionsInfo.end(); ++i)
	{
		Log::info() << i->first.first << "/" << i->first.second;
		if (i + 1 != functionsInfo.end())
			Log::info() << ", ";
	}
	Log::info() << std::endl;
}


static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(SQLFunctionsInfo)
