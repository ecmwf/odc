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


// Given the codec-initialising data, add the header on that is used to construct the
// codec.

size_t construct_full_header(std::vector<unsigned char>& header,
                           const std::string& codec_name,
                           const std::vector<unsigned char>& data,
                           bool bigEndian=false) {

    ASSERT(codec_name.size() < 255);

    // Include space for codec_name string, and the minmax data
    header.resize(4 + codec_name.length() + data.size());

    header[0] = 0;
    header[1] = 0;
    header[2] = 0;
    header[3] = 0;
    header[bigEndian ? 3 : 0] = static_cast<unsigned char>(codec_name.size());

    ::memcpy(&header[4], codec_name.c_str(), codec_name.length());

    ::memcpy(&header[4+codec_name.length()], &data[0], data.size());

    return 4 + codec_name.length();
}


// TODO: Test missing values

CASE("Constant values are constant") {

    // Data in little endian format.
    // "min" value is used for constants

    unsigned char data[] = {
        0x00, 0x00, 0x00, 0x00,                         // 0 = hasMissing
        0xb7, 0xe6, 0x87, 0xb4, 0x80, 0x65, 0xd2, 0x41, // min (little-endian: 1234567890.1234567)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = max
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = missingValue
    };

    // Construct codec directly

    MockReadDataHandle dh(data, sizeof(data)); // Skip name of codec

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

    std::vector<unsigned char> buffer;
    construct_full_header(buffer, "constant", std::vector<unsigned char>(data, data+sizeof(data)));

    MockReadDataHandle dh2(&buffer[0], buffer.size());
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
        0x00, 0x00, 0x00, 0x00,                         // 0 = hasMissing
         'h',  'i',  '-',  't',  'h',  'e',  'r',  'e', // min (big-endian: "hi-there")
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = max
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // = missingValue
    };

    // Construct codec directly

    MockReadDataHandle dh(data, sizeof(data)); // Skip name of codec

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

    std::vector<unsigned char> buffer;
    construct_full_header(buffer, "constant_string", std::vector<unsigned char>(data, data+sizeof(data)), true);

    MockReadDataHandle dh2(&buffer[0], buffer.size());
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


CASE("Constant integer or missing value behaves a bit oddly") {

    EXPECT(odb::MDI::integerMDI() == 2147483647);

    // little endian

    // Note that there is absolutely NOTHING that enforces that these are integers...
    // --> This test tests the generic case, with a double, which is odd
    // --> TODO: Really we ought to enforce integers for an integer case...

    unsigned char header_data[] = {
        0x01, 0x00, 0x00, 0x00,                         // 0 = hasMissing
//        0x00, 0x00, 0x80, 0x58, 0x34, 0x6f, 0xcd, 0x41, // min (little-endian: 987654321)
//        0x00, 0x00, 0x80, 0x58, 0x34, 0x6f, 0xcd, 0x41, // max == min
        0xad, 0x69, 0xfe, 0x58, 0x34, 0x6f, 0xcd, 0x41, // 987654321.9876 (big-endian)
        0xad, 0x69, 0xfe, 0x58, 0x34, 0x6f, 0xcd, 0x41,
        0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xdf, 0x41  // missingValue = 2147483647
    };

    std::vector<unsigned char> data(header_data, header_data+sizeof(header_data));
    data.push_back(0);
    data.push_back(0xff); // missing
    for (size_t i = 0; i < 255; i++) {
        data.push_back(static_cast<unsigned char>(i));
    }
    data.push_back(0xff); // missing

    // Construct codec directly

    MockReadDataHandle dh(&data[0], data.size()); // Skip name of codec

    eckit::ScopedPtr<Codec> c1;
    if (is_big_endian()) {
        c1.reset(new CodecConstantOrMissing<OtherByteOrder>);
        static_cast<CodecConstantOrMissing<OtherByteOrder>*>(c1.get())->load(&dh);
    } else {
        c1.reset(new CodecConstantOrMissing<SameByteOrder>);
        static_cast<CodecConstantOrMissing<SameByteOrder>*>(c1.get())->load(&dh);
    }
    c1->dataHandle(&dh);

    EXPECT(dh.position() == eckit::Offset(28));

    double baseValue = 987654321.9876;
//    double baseValue = 987654321;
    double decoded = c1->decode();
    EXPECT(baseValue == decoded);
    EXPECT(c1->decode() == odb::MDI::integerMDI()); // missing
    for (size_t i = 0; i < 255; i++) {
        double b = baseValue + i;
        double v = c1->decode();
        EXPECT(b == v);
    }
    EXPECT(c1->decode() == odb::MDI::integerMDI()); // missing

    // No further data should have been consumed from the data handle.
    EXPECT(dh.position() == eckit::Offset(28 + 258));

    // --
    // Construct codec from factory

    std::vector<unsigned char> buffer;
    size_t hdrSize = construct_full_header(buffer, "constant_or_missing", data);

    MockReadDataHandle dh2(&buffer[0], buffer.size());
    odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh2);
    odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh2);

    eckit::ScopedPtr<Codec> c2;
    if (is_big_endian()) {
        c2.reset(Codec::loadCodec(ds_other));
    } else {
        c2.reset(Codec::loadCodec(ds_same));
    }
    c2->dataHandle(&dh2);

    EXPECT(dh2.position() == eckit::Offset(hdrSize+28));

    decoded = c2->decode();
    EXPECT(baseValue == decoded);
    EXPECT(c2->decode() == odb::MDI::integerMDI()); // missing
    for (size_t i = 0; i < 255; i++) {
        double b = baseValue + i;
        double v = c2->decode();
        EXPECT(b == v);
    }
    EXPECT(c2->decode() == odb::MDI::integerMDI()); // missing

    EXPECT(dh2.position() == eckit::Offset(hdrSize + 28 + 258));
}


CASE("real constant or missing value is not quite constant") {

    EXPECT(odb::MDI::realMDI() == -2147483647);

    // little endian

    // TODO: Really something labelled constant ought to be actually constant...
    // Do this one big-endian just because.

    unsigned char header_data[] = {
        0x00, 0x00, 0x00, 0x01,                         // 0 = hasMissing
        0x41, 0xcd, 0x6f, 0x34, 0x58, 0xfe, 0x69, 0xad, // min = 987654321.9876 (big-endian)
        0x41, 0xcd, 0x6f, 0x34, 0x58, 0xfe, 0x69, 0xad, // max = 987654321.9876 (big-endian)
        0xc1, 0xdf, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00  // missingValue = -2147483647
    };

    std::vector<unsigned char> data(header_data, header_data+sizeof(header_data));
    data.push_back(0);
    data.push_back(0xff); // missing
    for (size_t i = 0; i < 255; i++) {
        data.push_back(static_cast<unsigned char>(i));
    }
    data.push_back(0xff); // missing

    // Construct codec directly

    MockReadDataHandle dh(&data[0], data.size()); // Skip name of codec

    eckit::ScopedPtr<Codec> c1;
    if (!is_big_endian()) {
        c1.reset(new CodecRealConstantOrMissing<OtherByteOrder>);
        static_cast<CodecRealConstantOrMissing<OtherByteOrder>*>(c1.get())->load(&dh);
    } else {
        c1.reset(new CodecRealConstantOrMissing<SameByteOrder>);
        static_cast<CodecRealConstantOrMissing<SameByteOrder>*>(c1.get())->load(&dh);
    }
    c1->dataHandle(&dh);

    EXPECT(dh.position() == eckit::Offset(28));

    double baseValue = 987654321.9876;
//    double baseValue = 987654321;
    double decoded = c1->decode();
    EXPECT(baseValue == decoded);
    EXPECT(c1->decode() == odb::MDI::realMDI()); // missing
    for (size_t i = 0; i < 255; i++) {
        double b = baseValue + i;
        double v = c1->decode();
        EXPECT(b == v);
    }
    EXPECT(c1->decode() == odb::MDI::realMDI()); // missing

    // No further data should have been consumed from the data handle.
    EXPECT(dh.position() == eckit::Offset(28 + 258));

    // --
    // Construct codec from factory

    std::vector<unsigned char> buffer;
    size_t hdrSize = construct_full_header(buffer, "real_constant_or_missing", data, true);

    MockReadDataHandle dh2(&buffer[0], buffer.size());
    odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh2);
    odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh2);

    eckit::ScopedPtr<Codec> c2;
    if (!is_big_endian()) {
        c2.reset(Codec::loadCodec(ds_other));
    } else {
        c2.reset(Codec::loadCodec(ds_same));
    }
    c2->dataHandle(&dh2);

    EXPECT(dh2.position() == eckit::Offset(hdrSize+28));

    decoded = c2->decode();
    EXPECT(baseValue == decoded);
    EXPECT(c2->decode() == odb::MDI::realMDI()); // missing
    for (size_t i = 0; i < 255; i++) {
        double b = baseValue + i;
        double v = c2->decode();
        EXPECT(b == v);
    }
    EXPECT(c2->decode() == odb::MDI::realMDI()); // missing

    EXPECT(dh2.position() == eckit::Offset(hdrSize + 28 + 258));
}


CASE("Character strings are 8-byte sequences coerced into being treated as doubles") {

    // n.b. there are no missing values for CodecChars

    // The values here are unused, and endianness is ignored for chars.

    unsigned char header_data[] = {
        0x00, 0x00, 0x00, 0x00,                         // 0 = hasMissing
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // min = 987654321.9876 (big-endian)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // max = 987654321.9876 (big-endian)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // missingValue = -2147483647
        0x00, 0x00, 0x00, 0x00                          // Unused 0 value required by chars codec
    };

    std::vector<unsigned char> data(header_data, header_data+sizeof(header_data));

    const char* s1 = "\0\0\0\0\0\0\0\0";
    const char* s2 = "hi-there";
    const char* s3 = "\0\xff\0\xff\0\xff\0\xff";
    const char* s4 = "a-string";
    const char* s5 = "\xff\xff\xff\xff\xff\xff\xff\xff";

    data.insert(data.end(), s1, s1+8);
    data.insert(data.end(), s2, s2+8);
    data.insert(data.end(), s3, s3+8);
    data.insert(data.end(), s4, s4+8);
    data.insert(data.end(), s5, s5+8);

    // The characters codec is independent of endianness, so test both.
    // 0 --> SameByteOrder,
    // 1 --> OtherByteOrder

    for (int i = 0; i < 2; i++) {

        // Construct codec directly

        MockReadDataHandle dh(&data[0], data.size()); // Skip name of codec

        eckit::ScopedPtr<Codec> c1;
        if (i == 0) {
            c1.reset(new CodecChars<SameByteOrder>);
            static_cast<CodecChars<SameByteOrder>*>(c1.get())->load(&dh);
        } else {
            c1.reset(new CodecChars<OtherByteOrder>);
            static_cast<CodecChars<OtherByteOrder>*>(c1.get())->load(&dh);
        }
        c1->dataHandle(&dh);

        EXPECT(dh.position() == eckit::Offset(32));

        eckit::Log::info() << "Take 2" << std::endl;

        double val = c1->decode();
        EXPECT(::memcmp(&val, s1, 8) == 0);
        val = c1->decode();
        EXPECT(::memcmp(&val, s2, 8) == 0);
        val = c1->decode();
        EXPECT(::memcmp(&val, s3, 8) == 0);
        val = c1->decode();
        EXPECT(::memcmp(&val, s4, 8) == 0);
        val = c1->decode();
        EXPECT(::memcmp(&val, s5, 8) == 0);

        // No further data should have been consumed from the data handle.
        EXPECT(dh.position() == eckit::Offset(32 + (8 * 5)));

        // --
        // Construct codec from factory

        // n.b. constrcut a different codec header depending on big/little endian
        std::vector<unsigned char> buffer;
        size_t hdrSize = construct_full_header(buffer, "chars", data, i != 0);

        MockReadDataHandle dh2(&buffer[0], buffer.size());
        odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh2);
        odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh2);

        eckit::ScopedPtr<Codec> c2;
        if (i == 0) {
            c2.reset(Codec::loadCodec(ds_same));
        } else {
            c2.reset(Codec::loadCodec(ds_other));
        }
        c2->dataHandle(&dh2);

        EXPECT(dh2.position() == eckit::Offset(hdrSize+32));

        val = c2->decode();
        EXPECT(::memcmp(&val, s1, 8) == 0);
        val = c2->decode();
        EXPECT(::memcmp(&val, s2, 8) == 0);
        val = c2->decode();
        EXPECT(::memcmp(&val, s3, 8) == 0);
        val = c2->decode();
        EXPECT(::memcmp(&val, s4, 8) == 0);
        val = c2->decode();
        EXPECT(::memcmp(&val, s5, 8) == 0);

        EXPECT(dh2.position() == eckit::Offset(hdrSize + 32 + (8 * 5)));
    }
}

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
