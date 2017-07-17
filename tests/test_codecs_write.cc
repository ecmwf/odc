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

// Some of the math.h/cmath functions are not clean when switching to C++11
#if __cplusplus <= 199711L
#include <math.h>
#else
#include <cmath>
#define fpclassify(x) std::fpclassify((x))
#define isinf(x) std::isinf((x))
#define isnan(x) std::isnan((x))
#define signbit(x) std::signbit((x))
#endif

using namespace eckit::testing;
using namespace odb::codec;
using odb::SameByteOrder;
using odb::OtherByteOrder;

// ------------------------------------------------------------------------------------------------------

class MockWriteDataHandle : public eckit::DataHandle {

public: // methods

    MockWriteDataHandle(const std::vector<unsigned char>& buffer) :
        buffer_(1024),
        position_(0) {}
    virtual ~MockWriteDataHandle() {}

    virtual void openForWrite(const eckit::Length&) {
        position_ = 0;
    }

    virtual long write(const void* p, long n) {
        ASSERT(n > 0);
        ASSERT(position_ + n <= buffer_.size());
        ::memcpy(&buffer_[position_], p, static_cast<size_t>(n));
        position_ += n;
        return n;
    }

    const void* get() const {
        return &buffer_[0];
    }

    virtual long read(void*, long) { NOTIMP; }
    virtual void close() {}
    virtual void rewind() { NOTIMP; }
    virtual eckit::Length estimate() { NOTIMP; }
    virtual eckit::Length openForRead() { NOTIMP; }
    virtual void openForAppend(const eckit::Length&) { NOTIMP; }
    virtual eckit::Offset position() { return eckit::Offset(position_); }
    virtual void print(std::ostream& s) const { s << "MockWriteDataHandle()"; }

private:

    std::vector<unsigned char> buffer_;
    size_t position_;
};

// ------------------------------------------------------------------------------------------------------

// TODO with codecs:
//
// i) Make them templated on the stream/datahandle directly
// ii) Construct them with a specific data handle/stream
// iii) Why are we casting data handles via a void* ???
// iv) Why are load/save not virtual functions?

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

size_t prepend_codec_selection_header(std::vector<unsigned char>& data,
                                    const std::string& codec_name,
                                    bool bigEndian=false) {

    data.insert(data.begin(), 4, 0);
    data[bigEndian ? 3 : 0] = static_cast<unsigned char>(codec_name.size());

    data.insert(data.begin() + 4, codec_name.begin(), codec_name.end());

    return 4 + codec_name.length();
}

// Normal write process:
//
//   i) Initialise codecs and set missing value if appropriate
//  ii) Append values into a buffor for multiple rows
// iii) Call gatherStats on codecs once for each row
//  iv) When block of rows is gathered, write header
//   v) Write the data, by calling encode on the data
// [vi) Re-initialise fresh codecs].


CASE("Constant values consume no space in the output data buffer") {

    // Data in little endian format.
    // "min" value is used for constants

    const char* expected_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                  // no missing value
        "\xb7\xe6\x87\xb4\x80\x65\xd2\x41",  // min (1234567890.1234567)
        "\xb7\xe6\x87\xb4\x80\x65\xd2\x41",  // max (1234567890.1234567)
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified

    };

    // Loop throumgh endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        MockWriteDataHandle dh(data); // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == is_big_endian()) {
            c.reset(new CodecConstant<SameByteOrder>);
        } else {
            c.reset(new CodecConstant<OtherByteOrder>);
        }

        c->hasMissing(false);
        c->missingValue(0.0);

        // Statistics in writing order

        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);

        // Encode the header to the data stream

        if (bigEndianOutput == is_big_endian()) {
            static_cast<CodecConstant<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecConstant<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(28));
        EXPECT(::memcmp(&data[0], dh.get(), data.size()) == 0);

        // Encode the data to wherever we want it (in reality would be after the header, via a buffer.).

        // n.b. We don't produce any data when encoding with this codec

        std::vector<unsigned char> buf(1024, 0);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
    }
}

//
//CASE("constant strings are constant") {
//
//    // Data in little endian format.
//    // "min" value is used for constants
//
//    // NOTE that strings are NOT swapped around when things are in the
//    // reverse byte order.
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                  // no missing value
//        "hi-there",                          // minimum supplies string
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i == 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//
//            // n.b. Don't swap string data around with endianness
//            if (bigEndianOutput && j != 1)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(new CodecConstantString<SameByteOrder>);
//                static_cast<CodecConstantString<SameByteOrder>*>(c.get())->load(&dh);
//            } else {
//                c.reset(new CodecConstantString<OtherByteOrder>);
//                static_cast<CodecConstantString<OtherByteOrder>*>(c.get())->load(&dh);
//            }
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            double val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//
//            // No further data should have been consumed from the data handle.
//            EXPECT(dh.position() == eckit::Offset(28));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, "constant_string", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            double val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//        }
//    }
//}
//
//
//CASE("Constant integer or missing value behaves a bit oddly") {
//
//    EXPECT(odb::MDI::integerMDI() == 2147483647);
//
//    // Note that there is absolutely NOTHING that enforces that these are integers...
//    // --> This test tests the generic case, with a double, which is odd
//    // --> TODO: Really we ought to enforce integers for an integer case...
//
//    // little
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x01\x00\x00\x00",                  // has missing value
////        "\x00\x00\x80\x58\x34\x6f\xcd\x41, // min (little-endian: 987654321)
////        "\x00\x00\x80\x58\x34\x6f\xcd\x41, // max == min
//        "\xad\x69\xfe\x58\x34\x6f\xcd\x41", // minimum value = 987654321.9876
//        "\xad\x69\xfe\x58\x34\x6f\xcd\x41", // maximum value
//        "\x00\x00\xc0\xff\xff\xff\xdf\x41"  // missingValue = 2147483647
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i == 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Insert the sequence of test values
//
//        data.push_back(0);
//        data.push_back(0xff); // missing
//        for (size_t i = 0; i < 255; i++) {
//            data.push_back(static_cast<unsigned char>(i));
//        }
//        data.push_back(0xff); // missing
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(new CodecConstantOrMissing<SameByteOrder>);
//                static_cast<CodecConstantOrMissing<SameByteOrder>*>(c.get())->load(&dh);
//            } else {
//                c.reset(new CodecConstantOrMissing<OtherByteOrder>);
//                static_cast<CodecConstantOrMissing<OtherByteOrder>*>(c.get())->load(&dh);
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            double baseValue = 987654321.9876;
//        //    double baseValue = 987654321;
//            double decoded = c->decode();
//            EXPECT(baseValue == decoded);
//            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing
//            for (size_t i = 0; i < 255; i++) {
//                double b = baseValue + i;
//                double v = c->decode();
//                EXPECT(b == v);
//            }
//            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing
//
//            EXPECT(dh.position() == eckit::Offset(28 + 258));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, "constant_or_missing", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize+28));
//
//            double baseValue = 987654321.9876;
//            double decoded = c->decode();
//            EXPECT(baseValue == decoded);
//            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing
//            for (size_t i = 0; i < 255; i++) {
//                double b = baseValue + i;
//                double v = c->decode();
//                EXPECT(b == v);
//            }
//            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + 258));
//        }
//    }
//}
//
//
//CASE("real constant or missing value is not quite constant") {
//
//    EXPECT(odb::MDI::realMDI() == -2147483647);
//
//    // TODO: Really something labelled constant ought to be actually constant...
//    // Do this one big-endian just because.
//
//    // BIG endian data
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x01",                  // has missing value
//        "\x41\xcd\x6f\x34\x58\xfe\x69\xad",  // min = 987654321.9876 (big-endian)
//        "\x41\xcd\x6f\x34\x58\xfe\x69\xad",  // max = 987654321.9876 (big-endian)
//        "\xc1\xdf\xff\xff\xff\xc0\x00\x00"   // missingValue = -2147483647
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i == 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (!bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Insert the sequence of test values
//
//        data.push_back(0);
//        data.push_back(0xff); // missing
//        for (size_t i = 0; i < 255; i++) {
//            data.push_back(static_cast<unsigned char>(i));
//        }
//        data.push_back(0xff); // missing
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(new CodecRealConstantOrMissing<SameByteOrder>);
//                static_cast<CodecRealConstantOrMissing<SameByteOrder>*>(c.get())->load(&dh);
//            } else {
//                c.reset(new CodecRealConstantOrMissing<OtherByteOrder>);
//                static_cast<CodecRealConstantOrMissing<OtherByteOrder>*>(c.get())->load(&dh);
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            double baseValue = 987654321.9876;
//        //    double baseValue = 987654321;
//            double decoded = c->decode();
//            EXPECT(baseValue == decoded);
//            EXPECT(c->decode() == odb::MDI::realMDI()); // missing
//            for (size_t i = 0; i < 255; i++) {
//                double b = baseValue + i;
//                double v = c->decode();
//                EXPECT(b == v);
//            }
//            EXPECT(c->decode() == odb::MDI::realMDI()); // missing
//
//            EXPECT(dh.position() == eckit::Offset(28 + 258));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, "real_constant_or_missing", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            double baseValue = 987654321.9876;
//            double decoded = c->decode();
//            EXPECT(baseValue == decoded);
//            EXPECT(c->decode() == odb::MDI::realMDI()); // missing
//            for (size_t i = 0; i < 255; i++) {
//                double b = baseValue + i;
//                double v = c->decode();
//                EXPECT(b == v);
//            }
//            EXPECT(c->decode() == odb::MDI::realMDI()); // missing
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + 258));
//        }
//    }
//}
//
//
//CASE("Character strings are 8-byte sequences coerced into being treated as doubles") {
//
//    // n.b. there are no missing values for CodecChars
//
//    // The values here are unused, and endianness is ignored for chars.
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                         // 0 = hasMissing
//        "\x00\x00\x00\x00\x00\x00\x00\x00",         // min = 987654321.9876 (big-endian)
//        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max = 987654321.9876 (big-endian)
//        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue = -2147483647
//        "\x00\x00\x00\x00",                         // Unused 0 value required by chars codec
//
//        // String data
//        "\0\0\0\0\0\0\0\0",
//        "hi-there",
//        "\0\xff\0\xff\0\xff\0\xff",
//        "a-string",
//        "\xff\xff\xff\xff\xff\xff\xff\xff",
//    };
//
//    // Loop throumgh endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i == 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0 || j == 4) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//
//            // n.b. Don't reverse the endianness of the string data.
//            if (bigEndianOutput && j < 5)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(new CodecChars<SameByteOrder>);
//                static_cast<CodecChars<SameByteOrder>*>(c.get())->load(&dh);
//            } else {
//                c.reset(new CodecChars<OtherByteOrder>);
//                static_cast<CodecChars<OtherByteOrder>*>(c.get())->load(&dh);
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(32));
//
//            double val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[5], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[6], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[7], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[8], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[9], 8) == 0);
//
//            EXPECT(dh.position() == eckit::Offset(32 + (8 * 5)));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, "chars", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 32));
//
//            double val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[5], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[6], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[7], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[8], 8) == 0);
//            val = c->decode();
//            EXPECT(::memcmp(&val, expected_data[9], 8) == 0);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 32 + (8 * 5)));
//        }
//    }
//}
//
//
//CASE("long floating point values can include the missing data value") {
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                  // no missing value
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // minimum unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified
//
//        // data to encode
//        "\x00\x00\x00\x00\x00\x00\x00\x00",   // 0.0
//        "\x53\xa4\x0c\x54\x34\x6f\x9d\x41",   // 123456789.0123456
//        "\x9b\xe6\x57\xb7\x80\x65\x02\xc2",   // -9876543210.9876
//        "\x00\x00\x00\x00\x00\x00\xf0\x7f",   // +inf
//        "\x00\x00\x00\x00\x00\x00\xf0\xff",   // -inf
//        "\x7f\xf7\xff\xff\xff\xff\xff\xff",   // NaN (signalling)
//        "\x7f\xff\xff\xff\xff\xff\xff\xff",   // NaN (quiet)
//        "\x00\x00\xc0\xff\xff\xff\xdf\xc1"    // -2147483647 (otherwise the missing value)
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i == 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(new CodecLongReal<SameByteOrder>);
//                static_cast<CodecLongReal<SameByteOrder>*>(c.get())->load(&dh);
//            } else {
//                c.reset(new CodecLongReal<OtherByteOrder>);
//                static_cast<CodecLongReal<OtherByteOrder>*>(c.get())->load(&dh);
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            EXPECT(c->decode() == 0);
//            EXPECT(c->decode() == 123456789.0123456);
//            EXPECT(c->decode() == -9876543210.9876);
//            double val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val > 0);
//            val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val < 0);
//            EXPECT(isnan(c->decode()));
//            EXPECT(isnan(c->decode()));
//            EXPECT(c->decode() == -2147483647);
//
//            EXPECT(dh.position() == eckit::Offset(28 + (8 * 8)));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, "long_real", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            EXPECT(c->decode() == 0);
//            EXPECT(c->decode() == 123456789.0123456);
//            EXPECT(c->decode() == -9876543210.9876);
//            double val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val > 0);
//            val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val < 0);
//            EXPECT(isnan(c->decode()));
//            EXPECT(isnan(c->decode()));
//            EXPECT(c->decode() == -2147483647);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + (8 * 8)));
//        }
//    }
//}
//
//
//CASE("short floating point values can include the missing data value") {
//
//    // Use a curious, custom missingValue to show it is being used.
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                  // no missing value
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // minimum unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
//        "\x04\x4f\xab\xa0\xe4\x4e\x91\x26",  // missing value = 6.54565456545599971850917315786e-123
//
//        // data to encode
//        "\x00\x00\x00\x00",   // 0.0
//        "\x12\xbf\x1f\x49",   // 654321.123
//        "\x00\x00\x80\x00",   // Smallest available, missing value for short_real (1.17549435082229e-38)
//        "\xff\xff\x7f\xff",   // Lowest available, missing value for short_real2 (-3.40282346638529e+38)
//        "\x00\x00\x80\x7f",   // +inf
//        "\x00\x00\x80\xff",   // -inf
//        "\xff\xff\xbf\x7f",   // NaN (signalling)
//        "\xff\xff\xff\x7f",   // NaN (quiet)
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 4; i++) {
//
//        bool bigEndianOutput = (i % 2 == 0);
//        bool secondCodec = (i > 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0 || j > 3) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                if (secondCodec) {
//                    c.reset(new CodecShortReal2<SameByteOrder>);
//                    static_cast<CodecShortReal2<SameByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecShortReal<SameByteOrder>);
//                    static_cast<CodecShortReal<SameByteOrder>*>(c.get())->load(&dh);
//                }
//            } else {
//                if (secondCodec) {
//                    c.reset(new CodecShortReal2<OtherByteOrder>);
//                    static_cast<CodecShortReal2<OtherByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecShortReal<OtherByteOrder>);
//                    static_cast<CodecShortReal<OtherByteOrder>*>(c.get())->load(&dh);
//                }
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            // n.b. == comparisons for floats as we are testing BIT reproducability of decoding
//            EXPECT(c->decode() == 0);
//            EXPECT(c->decode() == float(654321.123));
//            // Each of the two codecs has a different internal missing value. Check it is correctly recognised.
//            if (secondCodec) {
//                EXPECT(c->decode() == float(1.17549435082229e-38));
//                EXPECT(c->decode() == 6.54565456545599971850917315786e-123);
//            } else {
//                EXPECT(c->decode() == 6.54565456545599971850917315786e-123);
//                EXPECT(c->decode() == float(-3.40282346638529e+38));
//            }
//            double val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val > 0);
//            val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val < 0);
//            EXPECT(isnan(c->decode()));
//            EXPECT(isnan(c->decode()));
//
//            EXPECT(dh.position() == eckit::Offset(28 + (8 * 4)));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, secondCodec ? "short_real2" : "short_real", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            EXPECT(c->decode() == 0);
//            EXPECT(c->decode() == float(654321.123));
//            // Each of the two codecs has a different internal missing value. Check it is correctly recognised.
//            if (secondCodec) {
//                EXPECT(c->decode() == float(1.17549435082229e-38));
//                EXPECT(c->decode() == 6.54565456545599971850917315786e-123);
//            } else {
//                EXPECT(c->decode() == 6.54565456545599971850917315786e-123);
//                EXPECT(c->decode() == float(-3.40282346638529e+38));
//            }
//            double val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val > 0);
//            val = c->decode();
//            EXPECT(isinf(val));
//            EXPECT(val < 0);
//            EXPECT(isnan(c->decode()));
//            EXPECT(isnan(c->decode()));
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + (8 * 4)));
//        }
//    }
//}
//
//
//CASE("32bit integers are as-is") {
//
//    // Use a curious, custom missingValue to show it is being used.
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                  // no missing value
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // minimum unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified
//
//        // data to encode
//        "\x00\x00\x00\x00",   // 0.0
//        "\xff\xff\xff\xff",   // -1
//        "\xff\xff\xff\x7f",   // 2147483647  == largest
//        "\x00\x00\x00\x80",   // -2147483648 == smallest
//        "\x96\x28\x9c\xff"    // -6543210
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i == 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0 || j > 3) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(new CodecInt32<SameByteOrder>);
//                static_cast<CodecInt32<SameByteOrder>*>(c.get())->load(&dh);
//            } else {
//                c.reset(new CodecInt32<OtherByteOrder>);
//                static_cast<CodecInt32<OtherByteOrder>*>(c.get())->load(&dh);
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            EXPECT(c->decode() == 0);
//            EXPECT(c->decode() == -1);
//            EXPECT(c->decode() == 2147483647);
//            EXPECT(c->decode() == -2147483648);
//            EXPECT(c->decode() == -6543210);
//
//            EXPECT(dh.position() == eckit::Offset(28 + (5 * 4)));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, "int32", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            EXPECT(c->decode() == 0);
//            EXPECT(c->decode() == -1);
//            EXPECT(c->decode() == 2147483647);
//            EXPECT(c->decode() == -2147483648);
//            EXPECT(c->decode() == -6543210);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + (5 * 4)));
//        }
//    }
//}
//
//
//CASE("16bit integers are stored with an offset. This need not (strictly) be integral!!") {
//
//    // n.b. we use a non-standard, non-integral minimum to demonstrate the offset behaviour.
//
//    // Use a curious, custom missingValue to show it is being used.
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                  // no missing value
//        "\xcd\xcc\xcc\xcc\xcc\xdc\x5e\xc0",  // minimum = -123.45
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
//        "\x04\x4f\xab\xa0\xe4\x4e\x91\x26",  // missing value = 6.54565456545599971850917315786e-123
//
//        // data to encode
//        "\x00\x00",   // 0.0
//        "\xff\xff",   // 65535 and the missing value
//        "\xff\x7f",   // 32767 (no negatives)
//        "\x00\x80",   // 32768 (no negatives)
//        "\x39\x30"    // 12345
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 4; i++) {
//
//        bool bigEndianOutput = (i % 2 == 0);
//
//        bool withMissing = (i > 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0) ? 4 : (j > 3) ? 2 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                if (withMissing) {
//                    c.reset(new CodecInt16Missing<SameByteOrder>);
//                    static_cast<CodecInt16Missing<SameByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecInt16<SameByteOrder>);
//                    static_cast<CodecInt16<SameByteOrder>*>(c.get())->load(&dh);
//                }
//            } else {
//                if (withMissing) {
//                    c.reset(new CodecInt16Missing<OtherByteOrder>);
//                    static_cast<CodecInt16Missing<OtherByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecInt16<OtherByteOrder>);
//                    static_cast<CodecInt16<OtherByteOrder>*>(c.get())->load(&dh);
//                }
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            EXPECT(c->decode() == (double(-123.45) + 0));
//            if (withMissing) {
//                EXPECT(c->decode() == 6.54565456545599971850917315786e-123);
//            } else {
//                EXPECT(c->decode() == (double(-123.45) + 65535));
//            }
//            EXPECT(c->decode() == (double(-123.45) + 32767));
//            EXPECT(c->decode() == (double(-123.45) + 32768));
//            EXPECT(c->decode() == (double(-123.45) + 12345));
//
//            EXPECT(dh.position() == eckit::Offset(28 + (5 * 2)));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, withMissing ? "int16_missing" : "int16", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            EXPECT(c->decode() == (double(-123.45) + 0));
//            if (withMissing) {
//                EXPECT(c->decode() == 6.54565456545599971850917315786e-123);
//            } else {
//                EXPECT(c->decode() == (double(-123.45) + 65535));
//            }
//            EXPECT(c->decode() == (double(-123.45) + 32767));
//            EXPECT(c->decode() == (double(-123.45) + 32768));
//            EXPECT(c->decode() == (double(-123.45) + 12345));
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + (5 * 2)));
//        }
//    }
//}
//
//
//CASE("8bit integers are stored with an offset. This need not (strictly) be integral!!") {
//
//    // n.b. we use a non-standard, non-integral minimum to demonstrate the offset behaviour.
//
//    // Use a curious, custom missingValue to show it is being used.
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                  // no missing value
//        "\x00\x00\x00\x00\x80\x88\xb3\xc0",  // minimum = -5000.5
//        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
//        "\x04\x4f\xab\xa0\xe4\x4e\x91\x26",  // missing value = 6.54565456545599971850917315786e-123
//    };
//
//    // Loop through endiannesses for the source data
//
//    for (int i = 0; i < 4; i++) {
//
//        bool bigEndianOutput = (i % 2 == 0);
//
//        bool withMissing = (i > 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len = (j == 0) ? 4 : 8;
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//            if (bigEndianOutput)
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Add all of the data values
//
//        for (int n = 0; n < 256; n++) {
//            data.push_back(static_cast<unsigned char>(n));
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                if (withMissing) {
//                    c.reset(new CodecInt8Missing<SameByteOrder>);
//                    static_cast<CodecInt8Missing<SameByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecInt8<SameByteOrder>);
//                    static_cast<CodecInt8<SameByteOrder>*>(c.get())->load(&dh);
//                }
//            } else {
//                if (withMissing) {
//                    c.reset(new CodecInt8Missing<OtherByteOrder>);
//                    static_cast<CodecInt8Missing<OtherByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecInt8<OtherByteOrder>);
//                    static_cast<CodecInt8<OtherByteOrder>*>(c.get())->load(&dh);
//                }
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(28));
//
//            for (int n = 0; n < 255; n++) {
//                EXPECT(c->decode() == (double(-5000.5) + n));
//            }
//
//            EXPECT(c->decode() == (withMissing ? 6.54565456545599971850917315786e-123 : (-5000.5 + 255)));
//
//            EXPECT(dh.position() == eckit::Offset(28 + 256));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, withMissing ? "int8_missing" : "int8", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
//
//            for (int n = 0; n < 255; n++) {
//                EXPECT(c->decode() == (double(-5000.5) + n));
//            }
//
//            EXPECT(c->decode() == (withMissing ? 6.54565456545599971850917315786e-123 : (-5000.5 + 255)));
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + 256));
//        }
//    }
//}
//
//
//CASE("Character strings can be stored in a flat list, and indexed") {
//
//    // n.b. no missing values
//
//    const char* expected_data[] = {
//
//        // Codec header
//        "\x00\x00\x00\x00",                         // 0 = hasMissing
//        "\x00\x00\x00\x00\x00\x00\x00\x00",         // min unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max unspecified
//        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue unspecified
//
//        // How many strings are there in the table?
//        "\x06\x00\x00\x00",
//
//        // String data (prepended with lengths)
//        // length, data, "cnt (discarded)", index
//
//        "\x02\x00\x00\x00", "ab",           "\x00\x00\x00\x00", "\x03\x00\x00\x00", // This string is too short
//        "\x06\x00\x00\x00", "ghijkl",       "\x00\x00\x00\x00", "\x04\x00\x00\x00",
//        "\x08\x00\x00\x00", "mnopqrst",     "\x00\x00\x00\x00", "\x05\x00\x00\x00", // 8-byte length
//        "\x0c\x00\x00\x00", "uvwxyzabcdef", "\x00\x00\x00\x00", "\x01\x00\x00\x00", // too long
//        "\x08\x00\x00\x00", "ghijklmn",     "\x00\x00\x00\x00", "\x00\x00\x00\x00",
//        "\x08\x00\x00\x00", "opqrstuv",     "\x00\x00\x00\x00", "\x02\x00\x00\x00"
//    };
//
//    // Loop throumgh endiannesses for the source data
//
//    for (int i = 0; i < 2; i++) {
//
//        bool bigEndianOutput = (i % 2 == 0);
//
//        bool bits16 = (i > 1);
//
//        std::vector<unsigned char> data;
//
//        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
//            size_t len =
//                    (j < 5) ? ((j == 0 || j == 4) ? 4 : 8)
//                            : ((j+2) % 4 == 0 ? ::strlen(expected_data[j]) : 4);
//            data.insert(data.end(), expected_data[j], expected_data[j] + len);
//
//            // n.b. Don't reverse the endianness of the string data.
//            if (bigEndianOutput && !((j > 5) && ((j+2) % 4 == 0)))
//                std::reverse(data.end()-len, data.end());
//        }
//
//        // Which strings do we wish to decode (look at them in reverse. nb refers to index column)
//
//        for (int n = 5; n >= 0; n--) {
//            if (bits16 && bigEndianOutput)
//                data.push_back(0);
//            data.push_back(static_cast<unsigned char>(n));
//            if (bits16 && !bigEndianOutput)
//                data.push_back(0);
//        }
//
//        // Construct codec directly
//
//        {
//            MockWriteDataHandle dh(data); // Skip name of codec
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                if (bits16) {
//                    c.reset(new CodecInt16String<SameByteOrder>);
//                    static_cast<CodecInt16String<SameByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecInt8String<SameByteOrder>);
//                    static_cast<CodecInt8String<SameByteOrder>*>(c.get())->load(&dh);
//                }
//            } else {
//                if (bits16) {
//                    c.reset(new CodecInt16String<OtherByteOrder>);
//                    static_cast<CodecInt16String<OtherByteOrder>*>(c.get())->load(&dh);
//                } else {
//                    c.reset(new CodecInt8String<OtherByteOrder>);
//                    static_cast<CodecInt8String<OtherByteOrder>*>(c.get())->load(&dh);
//                }
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(148));
//
//            double val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "mnopqrst");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 6) == "ghijkl"); // silently works for shorter strings
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 2) == "ab"); // silently works for shorter strings
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "opqrstuv");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "uvwxyzab"); // gets truncated to 8
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "ghijklmn");
//
//            EXPECT(dh.position() == eckit::Offset(148 + (6 * (bits16 ? 2 : 1))));
//        }
//
//        // Construct codec from factory
//
//        size_t hdrSize = prepend_codec_selection_header(data, bits16 ? "1nt16_string" : "int8_string", bigEndianOutput);
//
//        {
//            MockWriteDataHandle dh(data);
//
//            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
//            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);
//
//            eckit::ScopedPtr<Codec> c;
//            if (bigEndianOutput == is_big_endian()) {
//                c.reset(Codec::loadCodec(ds_same));
//            } else {
//                c.reset(Codec::loadCodec(ds_other));
//            }
//            c->dataHandle(&dh);
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 148));
//
//            double val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "mnopqrst");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 6) == "ghijkl"); // silently works for shorter strings
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 2) == "ab"); // silently works for shorter strings
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "opqrstuv");
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "uvwxyzab"); // gets truncated to 8
//            val = c->decode();
//            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "ghijklmn");
//
//            EXPECT(dh.position() == eckit::Offset(hdrSize + 148 + (6 * (bits16 ? 2 : 1))));
//        }
//    }
//}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    return run_tests(argc, argv);
}
