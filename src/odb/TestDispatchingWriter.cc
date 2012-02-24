/// \file TestDispatchingWriter.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestDispatchingWriter.h"
#include "ToolFactory.h"
#include "CountTool.h"

#define SRC __FILE__, __LINE__

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
	const string fileName = "../odb2oda/2000010106/ECMA.odb";

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
	ksh("ls -l disp.*.*.oda", SRC);
}

void TestDispatchingWriter::setUp() {}

void TestDispatchingWriter::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

