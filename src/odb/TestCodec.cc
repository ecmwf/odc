/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCodec.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>

#include <strings.h>

using namespace std;

#include "odblib/oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "TestCase.h"

#include "TestCodec.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestCodec> _TestCodec("TestCodec");

TestCodec::TestCodec(int argc, char **argv)
: TestCase(argc, argv),
  codec_(0)
{}

TestCodec::~TestCodec() { delete codec_; }

void TestCodec::setUp() {}

class MockDataHandle : public DataHandle
{
public:
	MockDataHandle() : offset_(0) { bzero(buf_, sizeof(buf_)); }

	virtual void print(ostream& s) const { }
    virtual Length openForRead() { offset_ = 0; return sizeof(buf_); }
    virtual void openForWrite(const Length&) { offset_ = 0; }
    virtual void openForAppend(const Length&) {}

    virtual long read(void *p, long n)
	{
		memcpy(p, buf_ + offset_, n);
		offset_ += n;
		return n;
	}
    virtual long write(const void* p, long n) { return n; }
    virtual void close() {}
    virtual void rewind() {}
	virtual Length estimate() { return 0; }
	virtual Offset position() { return 0; }

	unsigned char *buffer() { return buf_; }

private:
	unsigned char buf_[1024];
	size_t offset_;
};

void TestCodec::test()
{
	Log::info() << fixed;

	string codecNames[] = {"short_real", "long_real"};
	for (size_t i = 0; i < sizeof(codecNames) / sizeof(string); i++)
	{
		Log::info() << "Testing codec " << codecNames[i] << endl;

		MockDataHandle dh;
		codec(odb::codec::Codec::findCodec<DataStream<SameByteOrder, DataHandle> >(codecNames[i], false));

		unsigned char *s = dh.buffer();
		
		double v = odb::MISSING_VALUE_REAL;
		Log::info() << "Encode: " << v << endl;

		s = codec().encode(s, v);

		codec().dataHandle(&dh);
		double d = codec().decode();
		Log::info() << "Decoded: " << d << endl;

		ASSERT(v == d);
	}
}

void TestCodec::tearDown() { delete codec_; codec_ = 0; }

} // namespace test 
} // namespace tool 
} // namespace odb 

