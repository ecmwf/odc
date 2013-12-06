/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCodecOptimization.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/io/DataHandle.h"

#include "odblib/odb_api.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Header.h"
#include "odblib/MetaData.h"
#include "odblib/Reader.h"
#include "odblib/ReaderIterator.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SelectIterator.h"
#include "odblib/TestCase.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/Writer.h"

#include "odb/TestCodecOptimization.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

const size_t numberOfColumns = 1;

ToolFactory<TestCodecOptimization> testCodecOptimization("TestCodecOptimization");

TestCodecOptimization::TestCodecOptimization(int argc, char **argv)
: TestCase(argc, argv)
{}

TestCodecOptimization::~TestCodecOptimization() { }

void TestCodecOptimization::test()
{
	testSelectIterator();
}

void TestCodecOptimization::setUp()
{
 {
	odb::Writer<> oda("TestCodecOptimization1.odb");
	odb::Writer<>::iterator row = oda.begin();

	MetaData& md = row->columns();
	md.setSize(numberOfColumns);

	row->setColumn(0, "x", odb::REAL);
	//row->setColumn(1, "y", odb::REAL);
	//row->setColumn(2, "z", odb::REAL);
	
	row->writeHeader();

	//for (size_t i = 1; i <= 2; i++)
	//{
	size_t i = 1;
	(*row)[0] = i; // col 0
	//(*row)[1] = i; // col 1
	//(*row)[2] = i; // col 2
	++row;
	//}
	row->close();
 }
}

void TestCodecOptimization::tearDown() { }

void TestCodecOptimization::testSelectIterator()
{

	odb::Reader reader("TestCodecOptimization1.odb");
	odb::Reader::iterator it = reader.begin();
	odb::MetaData& md = it->columns();

	Log::info() << "TestCodecOptimization::testSelectIterator: md = " << md << std::endl; 
	ASSERT(md.size() == numberOfColumns);

	string codecName = md[0]->coder().name();

	Log::info() << "TestCodecOptimization::testSelectIterator: codecName = " << codecName << std::endl; 

	ASSERT("All columns should be constant" && codecName == "constant");
	return;
}

} // namespace test 
} // namespace tool 
} // namespace odb 

