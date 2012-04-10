/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectIterator.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#include "odblib/oda.h"
#include "Tool.h"
#include "TestCase.h"
#include "TestSelectIterator.h"
#include "ToolFactory.h"




#include "eclib/PathName.h"
#include "eclib/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/MetaData.h"
#include "eclib/DataHandle.h"
#include "odblib/DataStream.h"
#include "odblib/RowsIterator.h"
#include "odblib/HashTable.h"
#include "eclib/Log.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/Header.h"
#include "odblib/Reader.h"
#include "eclib/DataHandle.h"
#include "odblib/SelectIterator.h"
#include "odblib/ReaderIterator.h"
#include "odblib/oda.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSelectIterator> _TestSelectIterator("TestSelectIterator");

TestSelectIterator::TestSelectIterator(int argc, char **argv)
: TestCase(argc, argv)
{}

TestSelectIterator::~TestSelectIterator() { }


const string SELECT  = "select * from \"test.oda\";";

unsigned char REF_DATA[] = 
{
	0x0,0x0,0x0,0x60,0x9b,0x5e,0x41,0x40,  // 34.7391
	0x0,0x0,0x0,0x20,0xcc,0xf,0x11,0xc0,   // -4.26543
	0x0,0x0,0x0,0x60,0x66,0x46,0x6b,0x40,  // 218.2
	0x0,0x0,0x0,0x0,0x77,0xc8,0x3b,0x40,   // 27.7831
	0x0,0x0,0x0,0x80,0xed,0xb,0xef,0xbf,   // -0.970206
	0x0,0x0,0x0,0xc0,0xcc,0xcc,0x6b,0x40,  // 222.4
	0x0,0x0,0x0,0x80,0x6,0xff,0x48,0x40,   // 49.9924
	0x0,0x0,0x0,0x80,0x81,0xec,0xeb,0x3f,  // 0.87262
	0x0,0x0,0x0,0x60,0x66,0x26,0x6b,0x40,  // 217.2
	0x0,0x0,0x0,0x0,0xc7,0x18,0x45,0x40,   // 42.1936
	0x0,0x0,0x0,0xc0,0x56,0x91,0xe7,0x3f,  // 0.736492
	0x0,0x0,0x0,0xc0,0xcc,0xec,0x6a,0x40,  // 215.4
	0x0,0x0,0x0,0x0,0xc7,0x18,0x45,0x40,   // 42.1936
	0x0,0x0,0x0,0xc0,0x56,0x91,0xe7,0xbf,  // -0.736492
	0x0,0x0,0x0,0xc0,0xcc,0xec,0x6a,0x40,  // 215.4
	0x0,0x0,0x0,0xa0,0xa5,0x7c,0x45,0x40,  // 42.9738
	0x0,0x0,0x0,0x40,0xc7,0x2,0xf8,0xbf,   // -1.50068
	0x0,0x0,0x0,0x60,0x66,0xe6,0x6a,0x40,  // 215.2
	0x0,0x0,0x0,0x60,0xf9,0xcb,0x45,0x40,  // 43.5935
	0x0,0x0,0x0,0x80,0x9,0x63,0x8,0xc0,    // -3.04836
	0x0,0x0,0x0,0x60,0x66,0xe6,0x6a,0x40,  // 215.2
	0x0,0x0,0x0,0x40,0xa3,0x22,0x36,0x40   // 22.1353
};
///
/// Tests problem fixed with p4 change 23687
///
void TestSelectIterator::testBug01()
{

#if defined(AIX) || defined(_HPUX_SOURCE)
	// Swap the data on this big-endian box.
	for (int i = 0; i < sizeof(REF_DATA) / 8; i++)
	{
		for (int j = 0; j < 4; j++)
			swap(REF_DATA[i * 8 + j], REF_DATA[(i + 1) * 8 - 1 - j]);
		//cout << *(reinterpret_cast<double *>(&REF_DATA[i * 8])) << endl;
	}
#endif

	const double *OBSVALUE = reinterpret_cast<const double*>(REF_DATA);

	//ksh("cd ../odb2oda && ./odb2oda era40:2000010106", SRC, false);
	const string SELECT = "select obsvalue from \"../odb2oda/2000010106/ECMA.odb\";";

	odb::Select oda(SELECT);
	size_t i = 0;
	for (odb::Select::iterator it = oda.begin();
		it != oda.end() && i < (sizeof(REF_DATA) / sizeof(double));
		++it, ++i) 
	{
		Log::info() << "TestSelectIterator::testBug01: it[" << i << "]=" << (*it)[0] << ", should be " << OBSVALUE[i] << endl;
		ASSERT( (*it)[0] == OBSVALUE[i] );
	}
}


void TestSelectIterator::test()
{
	testReaderIteratorForEach();
	testReaderIteratorLoop();
	testSelectIteratorForEach();
	testSelectIteratorLoop();
	testBug01();
}


template<class T> struct Count: public unary_function<T, void>
{
	Count() : counter(0) {}
	void operator() (T& row)
	{
		++counter;

		//long long i = row.integer(0);
		long long i = row[0];

		cerr << "Count::operator() counter=" << counter << ", i=" << i << endl; 
		ASSERT(i == counter);
		//cout << "col[0] (" << row.columns()[0]->name() << "): " << i << ", " ; 

		double n = row[1];
		n = n;
		//cout << "col[1] (" << row.columns()[1]->name() << "): " << n << " "  << endl;
	}
	long counter;
};


void TestSelectIterator::setUp()
{
	Log::debug() << "TestSelectIterator::setUp" << endl;

	Timer t("Writing test.oda");
	odb::Writer<> oda("test.oda");

	odb::Writer<>::iterator writer = oda.begin();
	writer->columns().setSize(3);

	(**writer).setColumn(0, "ifoo", odb::INTEGER);
	(**writer).setColumn(1, "nbar", odb::REAL);
	(**writer).setColumn(2, "string", odb::STRING);
	
	(**writer).writeHeader();

	for (size_t i = 1; i <= 10; i++)
	{
		writer->data()[0] = i; // col 0
		writer->data()[1] = i; // col 1
		++writer;
	}
}

void TestSelectIterator::tearDown() { }

void TestSelectIterator::testReaderIteratorForEach()
{
	Log::debug() << "TestSelectIterator::testReaderIteratorForEach" << endl;

	odb::Reader oda("test.oda");
	Timer t("for_each oda.reader Reading test.oda");
	Count<odb::Reader::row> counter1;
	counter1 = for_each(oda.begin(), oda.end(), counter1);

	//Log::info(Here()) << "for_each ReaderIterator: Read " << counter1.counter << " row(s)." << endl;

	ASSERT(counter1.counter == 10);
}

void TestSelectIterator::testReaderIteratorLoop()
{
	Log::debug() << "TestSelectIterator::testReaderIteratorLoop" << endl;

	odb::Reader oda("test.oda");
	int j = 0;
	for (odb::Reader::iterator it = oda.begin();
		it != oda.end();
		++it)
	{
		j++;

		const double * data = it->data();
		ASSERT(data);
		
		//int i = it->integer(0);
		int i = (*it)[0];
		double d = (*it)[1];

		//cout << i << "	" << d << endl;

		ASSERT( i == j );
		d = d;
	}
	ASSERT(j == 10);
}

void TestSelectIterator::testSelectIteratorLoop()
{
	odb::Select oda(SELECT, "test.oda");
	int i=0;
	for (odb::Select::iterator it = oda.begin();
		it != oda.end();
		++it) 
	{
		++i;
		//long v0 = it->integer(0);
		//double v1 = it->data(1);
		//cout << "v0 = " << v0 << ", v1 = " << v1 << endl;

		//ASSERT(it->integer(0) == i);
		ASSERT((*it)[0] == i);
		ASSERT((*it)[1] == i);
	}
	ASSERT(i == 10);
}

void TestSelectIterator::testSelectIteratorForEach()
{
	odb::Select oda(SELECT, "test.oda");
	Count<odb::Select::row> counter;
	counter = for_each(oda.begin(), oda.end(), counter);
	
	ASSERT(counter.counter == 10);
}


} // namespace test
} // namespace tool 
} // namespace odb 

