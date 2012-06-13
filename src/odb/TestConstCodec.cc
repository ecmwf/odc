/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestConstCodec.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eclib/PathName.h"
#include "eclib/Log.h"
#include "eclib/DataHandle.h"

#include "odblib/oda.h"
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

#include "odb/MockReader.h"
#include "odb/TestConstCodec.h"


using namespace odb;
using namespace odb::codec;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestConstCodec> _TestConstCodec("TestConstCodec");

const char *pies = "pies\0\0\0\0";

class MockReaderIterator : public odb::RowsReaderIterator 
{
public:
	MockReaderIterator()
	: columns_(1),
	  nRows_(2),
      n_(nRows_),
      data_(reinterpret_cast<double *>(const_cast<char *>(pies))),
      refCount_(0),
      noMore_(false)
	{
		odb::Column* col = columns_[0] = new odb::Column(columns_);
		ASSERT(col);

		col->name("foobar"); 
		col->type<DataStream<SameByteOrder, DataHandle> >(odb::STRING, false);
		col->hasMissing(false);
		//col->missingValue(0);
		odb::codec::HashTable *ht = new odb::codec::HashTable;
		ht->store("pies");
		odb::codec::CodecChars<odb::SameByteOrder> *codec = new odb::codec::CodecChars<odb::SameByteOrder>;
		codec->hashTable(ht);
		col->coder(codec);
	}

	odb::MetaData& columns() { return columns_; }

	bool isNewDataset() { return false; } // { return n_ == nRows_; }
	double* data() { return data_; }
	
	MockReaderIterator& operator++() { next(); return *this; }
	bool operator!=(const MockReaderIterator& o) { ASSERT(&o == 0); return nRows_ > 0; }
	const MockReaderIterator& end() { return *reinterpret_cast<MockReaderIterator*>(0); }

	bool next() { bool r = nRows_-- > 0; noMore_ = !r; return r; }

private:
	odb::MetaData columns_;
	int nRows_;
	int n_;
	double *data_;
public:
	int refCount_;
	bool noMore_;
};


TestConstCodec::TestConstCodec(int argc, char **argv)
: TestCase(argc, argv)
{}

TestConstCodec::~TestConstCodec() { }

void TestConstCodec::setUp()
{
	Timer t("Writing TestConstCodec.odb");
	odb::Writer<> oda("TestConstCodec.odb");

	typedef MockReader<MockReaderIterator> M;

	M mockReader;
	odb::Writer<>::iterator outit = oda.begin();

	M::iterator b = mockReader.begin();
	const M::iterator e = mockReader.end();
		
	outit->pass1(b, e);
}

void TestConstCodec::test()
{
	odb::Reader oda("TestConstCodec.odb");
	odb::Reader::iterator it = oda.begin();
	odb::Reader::iterator end = oda.end();

	Log::info() << it->columns() << endl;
	
	for ( ; it != end; ++it)
	{
		//Log::debug() << "TestConstCodec::test: '" << it.string(0) << "' (" << (*it)[0] << ")" << endl;
		ASSERT((*it)[0] == * ((double *) pies));
	}

	Log::debug() << "TestConstCodec::test: codec name is '" << it->columns()[0]->coder().name() << "'" << endl;
	ASSERT(it->columns()[0]->coder().name() == "constant_string");
}

void TestConstCodec::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

