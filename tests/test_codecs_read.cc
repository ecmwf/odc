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

class MockReadDataHandle : public eckit::DataHandle {

public: // methods

    MockReadDataHandle(const std::vector<unsigned char>& buffer) :
        buffer_(buffer),
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

    const std::vector<unsigned char>& buffer_;
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


size_t prepend_codec_selection_header(std::vector<unsigned char>& data,
                                    const std::string& codec_name,
                                    bool bigEndian=false) {

    data.insert(data.begin(), 4, 0);
    data[bigEndian ? 3 : 0] = static_cast<unsigned char>(codec_name.size());

    data.insert(data.begin() + 4, codec_name.begin(), codec_name.end());

    return 4 + codec_name.length();
}


struct CharReverse {
    CharReverse(const char* s, size_t len);
    ~CharReverse();


};



// TODO: Test missing values

CASE("Constant values are constant") {

    // Data in little endian format.
    // "min" value is used for constants

    const char* source_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                  // no missing value
        "\xb7\xe6\x87\xb4\x80\x65\xd2\x41",  // min (1234567890.1234567)
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified

    };

    // Loop throumgh endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianSource = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), source_data[j], source_data[j] + len);
            if (bigEndianSource)
                std::reverse(data.end()-len, data.end());
        }

        // Construct codec directly

        {
            MockReadDataHandle dh(data); // Skip name of codec

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(new CodecConstant<SameByteOrder>);
                static_cast<CodecConstant<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecConstant<OtherByteOrder>);
                static_cast<CodecConstant<OtherByteOrder>*>(c.get())->load(&dh);
            }

            EXPECT(dh.position() == eckit::Offset(28));

            EXPECT(c->decode() == 1234567890.1234567);
            EXPECT(c->decode() == 1234567890.1234567);
            EXPECT(c->decode() == 1234567890.1234567);
            EXPECT(c->decode() == 1234567890.1234567);

            // No further data should have been consumed from the data handle.
            EXPECT(dh.position() == eckit::Offset(28));
        }

        // Construct codec from factory

        size_t hdrSize = prepend_codec_selection_header(data, "constant", bigEndianSource);

        {
            MockReadDataHandle dh(data);

            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(Codec::loadCodec(ds_same));
            } else {
                c.reset(Codec::loadCodec(ds_other));
            }

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));

            EXPECT(c->decode() == 1234567890.1234567);
            EXPECT(c->decode() == 1234567890.1234567);
            EXPECT(c->decode() == 1234567890.1234567);
            EXPECT(c->decode() == 1234567890.1234567);

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
        }
    }
}


CASE("constant strings are constant") {

    // Data in little endian format.
    // "min" value is used for constants

    // NOTE that strings are NOT swapped around when things are in the
    // reverse byte order.

    const char* source_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                  // no missing value
        "hi-there",                          // minimum supplies string
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianSource = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), source_data[j], source_data[j] + len);

            // n.b. Don't swap string data around with endianness
            if (bigEndianSource && j != 1)
                std::reverse(data.end()-len, data.end());
        }

        // Construct codec directly

        {
            MockReadDataHandle dh(data); // Skip name of codec

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(new CodecConstantString<SameByteOrder>);
                static_cast<CodecConstantString<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecConstantString<OtherByteOrder>);
                static_cast<CodecConstantString<OtherByteOrder>*>(c.get())->load(&dh);
            }

            EXPECT(dh.position() == eckit::Offset(28));

            double val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
            val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
            val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
            val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");

            // No further data should have been consumed from the data handle.
            EXPECT(dh.position() == eckit::Offset(28));
        }

        // Construct codec from factory

        size_t hdrSize = prepend_codec_selection_header(data, "constant_string", bigEndianSource);

        {
            MockReadDataHandle dh(data);
            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(Codec::loadCodec(ds_same));
            } else {
                c.reset(Codec::loadCodec(ds_other));
            }

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));

            double val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
            val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
            val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");
            val = c->decode();
            EXPECT(std::string(reinterpret_cast<const char*>(&val), 8) == "hi-there");

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));
        }
    }
}


CASE("Constant integer or missing value behaves a bit oddly") {

    EXPECT(odb::MDI::integerMDI() == 2147483647);

    // Note that there is absolutely NOTHING that enforces that these are integers...
    // --> This test tests the generic case, with a double, which is odd
    // --> TODO: Really we ought to enforce integers for an integer case...

    // little

    const char* source_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
//        "\x00\x00\x80\x58\x34\x6f\xcd\x41, // min (little-endian: 987654321)
//        "\x00\x00\x80\x58\x34\x6f\xcd\x41, // max == min
        "\xad\x69\xfe\x58\x34\x6f\xcd\x41", // minimum value = 987654321.9876
        "\xad\x69\xfe\x58\x34\x6f\xcd\x41", // maximum value
        "\x00\x00\xc0\xff\xff\xff\xdf\x41"  // missingValue = 2147483647
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianSource = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), source_data[j], source_data[j] + len);
            if (bigEndianSource)
                std::reverse(data.end()-len, data.end());
        }

        // Insert the sequence of test values

        data.push_back(0);
        data.push_back(0xff); // missing
        for (size_t i = 0; i < 255; i++) {
            data.push_back(static_cast<unsigned char>(i));
        }
        data.push_back(0xff); // missing

        // Construct codec directly

        {
            MockReadDataHandle dh(data); // Skip name of codec

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(new CodecConstantOrMissing<SameByteOrder>);
                static_cast<CodecConstantOrMissing<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecConstantOrMissing<OtherByteOrder>);
                static_cast<CodecConstantOrMissing<OtherByteOrder>*>(c.get())->load(&dh);
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(28));

            double baseValue = 987654321.9876;
        //    double baseValue = 987654321;
            double decoded = c->decode();
            EXPECT(baseValue == decoded);
            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing
            for (size_t i = 0; i < 255; i++) {
                double b = baseValue + i;
                double v = c->decode();
                EXPECT(b == v);
            }
            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing

            // No further data should have been consumed from the data handle.
            EXPECT(dh.position() == eckit::Offset(28 + 258));
        }

        // Construct codec from factory

        size_t hdrSize = prepend_codec_selection_header(data, "constant_or_missing", bigEndianSource);

        {
            MockReadDataHandle dh(data);

            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(Codec::loadCodec(ds_same));
            } else {
                c.reset(Codec::loadCodec(ds_other));
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(hdrSize+28));

            double baseValue = 987654321.9876;
            double decoded = c->decode();
            EXPECT(baseValue == decoded);
            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing
            for (size_t i = 0; i < 255; i++) {
                double b = baseValue + i;
                double v = c->decode();
                EXPECT(b == v);
            }
            EXPECT(c->decode() == odb::MDI::integerMDI()); // missing

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + 258));
        }
    }
}


CASE("real constant or missing value is not quite constant") {

    EXPECT(odb::MDI::realMDI() == -2147483647);

    // TODO: Really something labelled constant ought to be actually constant...
    // Do this one big-endian just because.

    // BIG endian data

    const char* source_data[] = {

        // Codec header
        "\x00\x00\x00\x01",                  // has missing value
        "\x41\xcd\x6f\x34\x58\xfe\x69\xad",  // min = 987654321.9876 (big-endian)
        "\x41\xcd\x6f\x34\x58\xfe\x69\xad",  // max = 987654321.9876 (big-endian)
        "\xc1\xdf\xff\xff\xff\xc0\x00\x00"   // missingValue = -2147483647
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianSource = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), source_data[j], source_data[j] + len);
            if (!bigEndianSource)
                std::reverse(data.end()-len, data.end());
        }

        // Insert the sequence of test values

        data.push_back(0);
        data.push_back(0xff); // missing
        for (size_t i = 0; i < 255; i++) {
            data.push_back(static_cast<unsigned char>(i));
        }
        data.push_back(0xff); // missing

        // Construct codec directly

        {
            MockReadDataHandle dh(data); // Skip name of codec

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(new CodecRealConstantOrMissing<SameByteOrder>);
                static_cast<CodecRealConstantOrMissing<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecRealConstantOrMissing<OtherByteOrder>);
                static_cast<CodecRealConstantOrMissing<OtherByteOrder>*>(c.get())->load(&dh);
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(28));

            double baseValue = 987654321.9876;
        //    double baseValue = 987654321;
            double decoded = c->decode();
            EXPECT(baseValue == decoded);
            EXPECT(c->decode() == odb::MDI::realMDI()); // missing
            for (size_t i = 0; i < 255; i++) {
                double b = baseValue + i;
                double v = c->decode();
                EXPECT(b == v);
            }
            EXPECT(c->decode() == odb::MDI::realMDI()); // missing

            // No further data should have been consumed from the data handle.
            EXPECT(dh.position() == eckit::Offset(28 + 258));
        }

        // Construct codec from factory

        size_t hdrSize = prepend_codec_selection_header(data, "real_constant_or_missing", bigEndianSource);

        {
            MockReadDataHandle dh(data);
            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(Codec::loadCodec(ds_same));
            } else {
                c.reset(Codec::loadCodec(ds_other));
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));

            double baseValue = 987654321.9876;
            double decoded = c->decode();
            EXPECT(baseValue == decoded);
            EXPECT(c->decode() == odb::MDI::realMDI()); // missing
            for (size_t i = 0; i < 255; i++) {
                double b = baseValue + i;
                double v = c->decode();
                EXPECT(b == v);
            }
            EXPECT(c->decode() == odb::MDI::realMDI()); // missing

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + 258));
        }
    }
}


CASE("Character strings are 8-byte sequences coerced into being treated as doubles") {

    // n.b. there are no missing values for CodecChars

    // The values here are unused, and endianness is ignored for chars.

    const char* source_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                         // 0 = hasMissing
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // min = 987654321.9876 (big-endian)
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max = 987654321.9876 (big-endian)
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue = -2147483647
        "\x00\x00\x00\x00",                         // Unused 0 value required by chars codec

        // String data
        "\0\0\0\0\0\0\0\0",
        "hi-there",
        "\0\xff\0\xff\0\xff\0\xff",
        "a-string",
        "\xff\xff\xff\xff\xff\xff\xff\xff",
    };

    // Loop throumgh endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianSource = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len = (j == 0 || j == 4) ? 4 : 8;
            data.insert(data.end(), source_data[j], source_data[j] + len);

            // n.b. Don't reverse the endianness of the string data.
            if (bigEndianSource && j < 5)
                std::reverse(data.end()-len, data.end());
        }

        // Construct codec directly

        {
            MockReadDataHandle dh(data); // Skip name of codec

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(new CodecChars<SameByteOrder>);
                static_cast<CodecChars<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecChars<OtherByteOrder>);
                static_cast<CodecChars<OtherByteOrder>*>(c.get())->load(&dh);
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(32));

            double val = c->decode();
            EXPECT(::memcmp(&val, source_data[5], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[6], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[7], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[8], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[9], 8) == 0);

            // No further data should have been consumed from the data handle.
            EXPECT(dh.position() == eckit::Offset(32 + (8 * 5)));
        }

        // Construct codec from factory

        size_t hdrSize = prepend_codec_selection_header(data, "chars", bigEndianSource);

        {
            MockReadDataHandle dh(data);

            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(Codec::loadCodec(ds_same));
            } else {
                c.reset(Codec::loadCodec(ds_other));
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(hdrSize + 32));

            double val = c->decode();
            EXPECT(::memcmp(&val, source_data[5], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[6], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[7], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[8], 8) == 0);
            val = c->decode();
            EXPECT(::memcmp(&val, source_data[9], 8) == 0);

            EXPECT(dh.position() == eckit::Offset(hdrSize + 32 + (8 * 5)));
        }
    }
}


CASE("long floating point values can include the missing data value") {

    // n.b. there are no missing values for CodecChars

    const char* source_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                  // no missing value
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // minimum unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // maximum unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified

        // data to encode
        "\x00\x00\x00\x00\x00\x00\x00\x00",   // 0.0
        "\x53\xa4\x0c\x54\x34\x6f\x9d\x41",   // 123456789.0123456
        "\x9b\xe6\x57\xb7\x80\x65\x02\xc2",   // -9876543210.9876
        "\x00\x00\x00\x00\x00\x00\xf0\x7f",   // +inf
        "\x00\x00\x00\x00\x00\x00\xf0\xff",   // -inf
        "\x7f\xf7\xff\xff\xff\xff\xff\xff",   // NaN (signalling)
        "\x7f\xff\xff\xff\xff\xff\xff\xff",   // NaN (quiet)
        "\x00\x00\xc0\xff\xff\xff\xdf\xc1"    // -2147483647 (otherwise the missing value)
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianSource = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), source_data[j], source_data[j] + len);
            if (bigEndianSource)
                std::reverse(data.end()-len, data.end());
        }

        // Construct codec directly

        {
            MockReadDataHandle dh(data); // Skip name of codec

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(new CodecLongReal<SameByteOrder>);
                static_cast<CodecLongReal<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecLongReal<OtherByteOrder>);
                static_cast<CodecLongReal<OtherByteOrder>*>(c.get())->load(&dh);
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(28));

            EXPECT(c->decode() == 0);
            EXPECT(c->decode() == 123456789.0123456);
            EXPECT(c->decode() == -9876543210.9876);
            double val = c->decode();
            EXPECT(isinf(val));
            EXPECT(val > 0);
            val = c->decode();
            EXPECT(isinf(val));
            EXPECT(val < 0);
            EXPECT(isnan(c->decode()));
            EXPECT(isnan(c->decode()));
            EXPECT(c->decode() == -2147483647);

            // No further data should have been consumed from the data handle.
            EXPECT(dh.position() == eckit::Offset(28 + (8 * 8)));
        }

        // Construct codec from factory

        size_t hdrSize = prepend_codec_selection_header(data, "long_real", bigEndianSource);

        {
            MockReadDataHandle dh(data);

            odb::DataStream<odb::SameByteOrder, eckit::DataHandle> ds_same(dh);
            odb::DataStream<odb::OtherByteOrder, eckit::DataHandle> ds_other(dh);

            eckit::ScopedPtr<Codec> c;
            if (bigEndianSource == is_big_endian()) {
                c.reset(Codec::loadCodec(ds_same));
            } else {
                c.reset(Codec::loadCodec(ds_other));
            }
            c->dataHandle(&dh);

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28));

            EXPECT(c->decode() == 0);
            EXPECT(c->decode() == 123456789.0123456);
            EXPECT(c->decode() == -9876543210.9876);
            double val = c->decode();
            EXPECT(isinf(val));
            EXPECT(val > 0);
            val = c->decode();
            EXPECT(isinf(val));
            EXPECT(val < 0);
            EXPECT(isnan(c->decode()));
            EXPECT(isnan(c->decode()));
            EXPECT(c->decode() == -2147483647);

            EXPECT(dh.position() == eckit::Offset(hdrSize + 28 + (8 * 8)));
        }
    }
}

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
