/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/testing/Test.h"
#include "eckit/io/DataHandle.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/eckit_ecbuild_config.h"

#include "odb_api/Codec.h"

#include <time.h>
#include <stdlib.h>

using namespace eckit::testing;
using namespace odb::codec;
using odb::SameByteOrder;
using odb::OtherByteOrder;

// ------------------------------------------------------------------------------------------------------

class MockReadDataHandle : public eckit::DataHandle {

public: // methods

    MockReadDataHandle(const void* data, long len) :
        buffer_(static_cast<const char*>(data), static_cast<const char*>(data) + len),
        position_(0) {}
    virtual ~MockReadDataHandle() {}

    virtual eckit::Length openForRead() {
        position_ = 0;
        return static_cast<long long>(buffer_.size());
    }

    virtual long read(void *p, long n) {
//        eckit::Log::info() << "READ: " << n << ", " << position_ << std::endl;
        ASSERT(n >= 0);
        ASSERT(position_ + n <= buffer_.size());
        ::memcpy(p, &buffer_[position_], static_cast<size_t>(n));
        position_ += n;
        return n;
    }

    virtual long write(const void*, long) { NOTIMP; }
    virtual void close() {}
    virtual void rewind() { NOTIMP; }
    virtual eckit::Length estimate() { NOTIMP; }
    virtual void openForWrite(const eckit::Length&) { NOTIMP; }
    virtual void openForAppend(const eckit::Length&) { NOTIMP; }
    virtual eckit::Offset position() { return eckit::Offset(position_); }
    virtual void print(std::ostream& s) const { s << "MockReadDataHandle()"; }

private:

    std::vector<char> buffer_;
    size_t position_;
};

// ------------------------------------------------------------------------------------------------------

// TODO with codecs:
//
// i) Make them templated on the stream/datahandle directly
// ii) Construct them with a specific data handle/stream
// iii) Why are we casting data handles via a void* ???

// A quick helper function, to minimised #ifdefs
bool is_big_endian() {
#ifdef EC_BIG_ENDIAN
    return true;
#endif
#ifdef EC_LITTLE_ENDIAN
    return false;
#endif
    ASSERT(false);
}


// TODO: Test missing values

CASE("Constant values are constant") {

    // Data in little endian format.
    // "min" value is used for constants

    unsigned char data[] = {
        0x08, 0x00, 0x00, 0x00,                         // 8 chars in name
        'c', 'o', 'n', 's', 't', 'a', 'n', 't',         // constant
        0x00, 0x00, 0x00, 0x00,                         // 0 = hasMissing
        0xb7, 0xe6, 0x87, 0xb4, 0x80, 0x65, 0xd2, 0x41, // min (little-endian: 1234567890.1234567)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = max
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = missingValue
    };

    // Construct codec directly

    MockReadDataHandle dh(data+12, sizeof(data)-12); // Skip name of codec

    eckit::ScopedPtr<Codec> c1;
    if (is_big_endian()) {
        c1.reset(new CodecConstant<OtherByteOrder>);
        static_cast<CodecConstant<OtherByteOrder>*>(c1.get())->load(&dh);
    } else {
        c1.reset(new CodecConstant<SameByteOrder>);
        static_cast<CodecConstant<SameByteOrder>*>(c1.get())->load(&dh);
    }

    EXPECT(dh.position() == eckit::Offset(28));

    EXPECT(c1->decode() == 1234567890.1234567);
    EXPECT(c1->decode() == 1234567890.1234567);
    EXPECT(c1->decode() == 1234567890.1234567);
    EXPECT(c1->decode() == 1234567890.1234567);

    // No further data should have been consumed from the data handle.
    EXPECT(dh.position() == eckit::Offset(28));

    // --
    // Construct codec from factory

    MockReadDataHandle dh2(data, sizeof(data));
    odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh2);
    odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh2);

    eckit::ScopedPtr<Codec> c2;
    if (is_big_endian()) {
        c2.reset(Codec::loadCodec(ds_other));
    } else {
        c2.reset(Codec::loadCodec(ds_same));
    }

    EXPECT(dh2.position() == eckit::Offset(40));

    EXPECT(c2->decode() == 1234567890.1234567);
    EXPECT(c2->decode() == 1234567890.1234567);
    EXPECT(c2->decode() == 1234567890.1234567);
    EXPECT(c2->decode() == 1234567890.1234567);

    EXPECT(dh2.position() == eckit::Offset(40));
}


CASE("constant strings are constant") {

    // NOTE that strings are NOT swapped around when things are in the
    // reverse byte order.
    // --> Here we supply the data in BIG endian format, just to be perverse.

    // Big endian data

    unsigned char data[] = {
        0x00, 0x00, 0x00, 0x0F,                         // 15 chars in name
        'c', 'o', 'n', 's', 't', 'a', 'n', 't', '_', 's', 't', 'r', 'i', 'n', 'g', // constant_string
        0x00, 0x00, 0x00, 0x00,                         // 0 = hasMissing
         'h',  'i',  '-',  't',  'h',  'e',  'r',  'e', // min (big-endian: "hi-there")
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = max
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = missingValue
    };

    // Construct codec directly

    MockReadDataHandle dh(data+19, sizeof(data)-19); // Skip name of codec

    eckit::ScopedPtr<Codec> c1;
    if (!is_big_endian()) {
        c1.reset(new CodecConstantString<OtherByteOrder>);
        static_cast<CodecConstantString<OtherByteOrder>*>(c1.get())->load(&dh);
    } else {
        c1.reset(new CodecConstantString<SameByteOrder>);
        static_cast<CodecConstantString<SameByteOrder>*>(c1.get())->load(&dh);
    }

    EXPECT(dh.position() == eckit::Offset(28));

    double val = c1->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
    val = c1->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
    val = c1->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
    val = c1->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");

    // No further data should have been consumed from the data handle.
    EXPECT(dh.position() == eckit::Offset(28));

    // --
    // Construct codec from factory

    MockReadDataHandle dh2(data, sizeof(data));
    odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh2);
    odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh2);

    eckit::ScopedPtr<Codec> c2;
    if (!is_big_endian()) {
        c2.reset(Codec::loadCodec(ds_other));
    } else {
        c2.reset(Codec::loadCodec(ds_same));
    }

    EXPECT(dh2.position() == eckit::Offset(47));

    val = c2->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
    val = c2->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
    val = c2->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
    val = c2->decode();
    EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");

    EXPECT(dh2.position() == eckit::Offset(47));

}

// constant_or_missing
// real_constant
// chars
// long_real
// short_real
// short_real2
// int32
// int16
// int8
// int16_missing
// int8_missing
// int16_string
// int8_string

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {


    srand(time(NULL));
    return run_tests(argc, argv);
}
