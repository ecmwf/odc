/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestIntegerValues.h
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

#include "TestIntegerValues.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestIntegerValues> _TestIntegerValues("TestIntegerValues");

TestIntegerValues::TestIntegerValues(int argc, char **argv)
: TestCase(argc, argv)
{}

TestIntegerValues::~TestIntegerValues() { }

void TestIntegerValues::setUp() {}


void TestIntegerValues::test()
{
	//Log::info() << fixed;
 //////////////////////////////           ODB from MARS             //////////////////////////////    

	const string fileNameOdb="TestIntegerValues.odb";

	odb::Reader odb(fileNameOdb);
	odb::Reader::iterator it = odb.begin();

	for (unsigned int i=0; i < it->columns().size(); ++i) {
		cout << "Name = " << it->columns()[i]->name() << " " ;
	}

	cout << endl;
	int nrows=0;
	for(; it != odb.end(); ++it)
	{
		++nrows;
		for (size_t i=0; i < it->columns().size(); ++i)
		{
			//float nr = ((*it)[i]); /// <- WRONG!
			double nr = ((*it)[i]);
			switch(it->columns()[i]->type())
			{
			case odb::INTEGER:
			case odb::BITFIELD:
				cout <<  static_cast<int>(nr) << " ";
				//cout <<  "* should be: " << it->integer(i) << " ";
				break;
			case odb::REAL:
				cout <<  nr << " ";
				break;
			case odb::IGNORE:
			default:
				ASSERT("Unknown type" && false);
				break;
			}
 		}
 		cout << endl;
	}
}

void TestIntegerValues::tearDown() { }

} // namespace test 
} // namespace tool 
} // namespace odb 

