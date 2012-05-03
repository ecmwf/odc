/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestDispatchingWriter.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "odblib/oda.h"
#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"
#include "odblib/CountTool.h"

#include "odb/TestDispatchingWriter.h"


namespace odb {
namespace tool {
namespace test {

ToolFactory<TestDispatchingWriter> _TestDispatchingWriter("TestDispatchingWriter");

TestDispatchingWriter::TestDispatchingWriter(int argc, char **argv)
: TestCase(argc, argv)
{}

TestDispatchingWriter::~TestDispatchingWriter() { }

/// Tests DispatchingWriter
///
void TestDispatchingWriter::test()
{
	const string fileName = "2000010106/ECMA.odb";

	odb::Reader oda(fileName);
	odb::Reader::iterator it = oda.begin();
	const odb::Reader::iterator end = oda.end();
	ASSERT(it->columns().size() > 0);

	odb::DispatchingWriter writer("disp.{obstype}.{sensor}.oda");
	odb::DispatchingWriter::iterator wi = writer.begin();
	unsigned long long n1 = wi->pass1(it, end);
	wi->close();
	
	unsigned long long sum = 0;
	vector<PathName> files = (**wi).getFiles();
	for (size_t i = 0; i < files.size(); ++i)
	{
		unsigned long long n = CountTool::rowCount(files[i]);
		Log::info() << i << ". " << files[i] << ": " << n << endl;
		sum += n;
	}

	ASSERT(n1 == sum);
	ksh("ls -l disp.*.*.oda", Here());
}

void TestDispatchingWriter::setUp() {}

void TestDispatchingWriter::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

