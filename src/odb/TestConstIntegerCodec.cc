/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestConstIntegerCodec.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/io/DataHandle.h"

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
#include "odb/TestConstIntegerCodec.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestConstIntegerCodec> _TestConstIntegerCodec("TestConstIntegerCodec");

const long the_const_value = 20090624;

class MockReaderIterator2 : public odb::RowsReaderIterator 
{
public:
	MockReaderIterator2() : columns_(1), nRows_(2), n_(nRows_), data_(the_const_value), refCount_(0), noMore_(false)
	{
		odb::Column* col = columns_[0] = new odb::Column(columns_);
		ASSERT(col);

		col->name("date"); 
		col->type<DataStream<SameByteOrder, DataHandle> >(odb::INTEGER, false);
		col->hasMissing(false);
		odb::codec::CodecInt32<odb::SameByteOrder> *codec = new odb::codec::CodecInt32<odb::SameByteOrder>;
		col->coder(codec);
	}

	odb::MetaData& columns() { return columns_; }

	bool isNewDataset() { return false; } // { return n_ == nRows_; }
	double* data() { return &data_; }

	bool next() { return !(noMore_ = nRows_-- <= 0); }

private:
	odb::MetaData columns_;
	int nRows_;
	int n_;
	double data_;

public:
	int refCount_;
	bool noMore_;
};


TestConstIntegerCodec::TestConstIntegerCodec(int argc, char **argv)
: TestCase(argc, argv)
{}

TestConstIntegerCodec::~TestConstIntegerCodec() { }

void TestConstIntegerCodec::setUp()
{
	Timer t("Writing test_integer_const.odb");
	odb::Writer<> oda("test_integer_const.odb");

	typedef MockReader<MockReaderIterator2> M;
	M reader;

	odb::Writer<>::iterator outit = oda.begin();

	M::iterator b = reader.begin();
	const M::iterator e = reader.end();
	outit->pass1(b, e);
}

void TestConstIntegerCodec::test()
{
	odb::Reader oda("test_integer_const.odb");
	odb::Reader::iterator it = oda.begin();
	odb::Reader::iterator end = oda.end();

	Log::info() << it->columns() << endl;
	
	for ( ; it != end; ++it)
		ASSERT((*it)[0] == the_const_value);

	Log::debug() << "TestConstIntegerCodec::test: codec name is '" << it->columns()[0]->coder().name() << "'" << endl;
	ASSERT(it->columns()[0]->coder().name() == "constant");
}

void TestConstIntegerCodec::tearDown() {}

} // namespace test 
} // namespace tool 
} // namespace odb 

