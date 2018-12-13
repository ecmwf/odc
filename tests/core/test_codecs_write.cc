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
#include "eckit/system/SystemInfo.h"
#include "eckit/io/DataHandle.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/eckit_ecbuild_config.h"

#include "odc/Codec.h"

#include "MockDataHandles.h"

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
using namespace odc::codec;
using odc::SameByteOrder;
using odc::OtherByteOrder;

// ------------------------------------------------------------------------------------------------------

// TODO with codecs:
//
//   i) Make them templated on the stream/datahandle directly
//  ii) Construct them with a specific data handle/stream
// iii) Why are we casting data handles via a void* ???
//  iv) Why are load/save not virtual functions?
//   v) We should ASSERT() that encoded data is constant for constant codecs. Currently it is just ignored.

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

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecConstant<SameByteOrder>);
        } else {
            c.reset(new CodecConstant<OtherByteOrder>);
        }

        c->missingValue(0.0);

        // Statistics in writing order

        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);
        c->gatherStats(1234567890.1234567);

        EXPECT(!c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecConstant<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecConstant<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(28));
        EXPECT(::memcmp(&data[0], dh.getBuffer(), data.size()) == 0);

        // Encode the data to wherever we want it (in reality would be after the header, via a buffer.).

        // n.b. We don't produce any data when encoding with this codec

        std::vector<unsigned char> buf(1024, 0);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);
        EXPECT(c->encode(&buf[0], 1234567890.1234567) == &buf[0]);

        for (size_t n = 0; n < buf.size(); n++)
            EXPECT(buf[n] == 0);
    }
}


CASE("constant strings consume no output data space") {

    // Data in little endian format.
    // "min" value is used for constants

    // NOTE that strings are NOT swapped around when things are in the
    // reverse byte order.

    const char* expected_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                  // no missing value
        "hi-there",                          // minimum supplies string
        "hi-there",                          // maximum unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",  // missing value unspecified
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            // n.b. Don't swap string data around with endianness
        }

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecConstantString<SameByteOrder>);
        } else {
            c.reset(new CodecConstantString<OtherByteOrder>);
        }

        c->missingValue(0.0);

        // Statistics in writing order

        const char* str = "hi-there";
        c->gatherStats(*reinterpret_cast<const double*>(str));
        c->gatherStats(*reinterpret_cast<const double*>(str));
        c->gatherStats(*reinterpret_cast<const double*>(str));
        c->gatherStats(*reinterpret_cast<const double*>(str));

        EXPECT(!c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecConstantString<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecConstantString<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(28));
        EXPECT(::memcmp(&data[0], dh.getBuffer(), data.size()) == 0);

        // Encode the data to wherever we want it (in reality would be after the header, via a buffer.).

        // n.b. We don't produce any data when encoding with this codec

        std::vector<unsigned char> buf(1024, 0);
        EXPECT(c->encode(&buf[0], *reinterpret_cast<const double*>(str)) == &buf[0]);
        EXPECT(c->encode(&buf[0], *reinterpret_cast<const double*>(str)) == &buf[0]);
        EXPECT(c->encode(&buf[0], *reinterpret_cast<const double*>(str)) == &buf[0]);
        EXPECT(c->encode(&buf[0], *reinterpret_cast<const double*>(str)) == &buf[0]);

        for (size_t n = 0; n < buf.size(); n++)
            EXPECT(buf[n] == 0);
    }
}


CASE("Constant integer or missing value behaves a bit oddly") {

    EXPECT(odc::MDI::integerMDI() == 2147483647);

    // Note that there is absolutely NOTHING that enforces that these are integers...
    // --> This test tests the generic case, with a double, which is odd
    // --> TODO: Really we ought to enforce integers for an integer case...

    double customMissingValue = 2222222222;
    double baseValue = 987654321.9876;
    const size_t expectedHdrSize = 28;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
//        "\x00\x00\x80\x58\x34\x6f\xcd\x41, // min (little-endian: 987654321)
//        "\x00\x00\x80\x58\x34\x6f\xcd\x41, // max == min
        "\xad\x69\xfe\x58\x34\x6f\xcd\x41", // minimum value = 987654321.9876
        "\xad\x69\xfe\xd7\x34\x6f\xcd\x41", // maximum value = 987654321.9876 + 254
        "\x00\x00\xc0\x71\x8d\x8e\xe0\x41"   // missingValue = -2222222222
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        // Insert the sequence of test values

        data.push_back(0);
        data.push_back(0xff); // missing
        for (size_t n = 0; n < 255; n++) {
            data.push_back(static_cast<unsigned char>(n));
        }
        data.push_back(0xff); // missing

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecConstantOrMissing<SameByteOrder>);
        } else {
            c.reset(new CodecConstantOrMissing<OtherByteOrder>);
        }

        c->missingValue(customMissingValue);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(baseValue + 0);
        c->gatherStats(customMissingValue); // missing
        for (size_t n = 0; n < 255; n++) {
            c->gatherStats(baseValue + n);
        }
        c->gatherStats(customMissingValue); // missing

        // Detects that we have added a missing value
        EXPECT(c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecConstantOrMissing<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecConstantOrMissing<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it (in reality would be after the header, via a buffer.).
        // Expect one byte per element.

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), baseValue + 0)) == (dh.get() + 1));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 1));
        dh.set(posNext);

        for (size_t n = 0; n < 255; n++) {
            EXPECT((posNext = c->encode(dh.get(), baseValue + n)) == (dh.get() + 1));
            dh.set(posNext);
        }

        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 1));
        dh.set(posNext);

        // Check we have the data we expect

        size_t nelem = 258;
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + nelem));

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + nelem) == 0);
    }
}


CASE("real constant or missing value is not quite constant") {

    EXPECT(odc::MDI::realMDI() == -2147483647);

    // TODO: Really something labelled constant ought to be actually constant...
    // Do this one big-endian just because.

    double customMissingValue = -2222222222;
    double baseValue = 987654321.9876;
    const size_t expectedHdrSize = 28;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
        "\xad\x69\xfe\x58\x34\x6f\xcd\x41", // minimum value = 987654321.9876
        "\xad\x69\xfe\xd7\x34\x6f\xcd\x41", // maximum value = 987654321.9876 + 254
        "\x00\x00\xc0\x71\x8d\x8e\xe0\xc1"   // missingValue = -2222222222
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        // Insert the sequence of test values

        data.push_back(0);
        data.push_back(0xff); // missing
        for (size_t n = 0; n < 255; n++) {
            data.push_back(static_cast<unsigned char>(n));
        }
        data.push_back(0xff); // missing

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecRealConstantOrMissing<SameByteOrder>);
        } else {
            c.reset(new CodecRealConstantOrMissing<OtherByteOrder>);
        }

        c->missingValue(customMissingValue);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(baseValue + 0);
        c->gatherStats(customMissingValue);
        for (size_t n = 0; n < 255; n++) {
            c->gatherStats(baseValue + n);
        }
        c->gatherStats(customMissingValue);

        // Detect that we have added a missing value
        EXPECT(c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecRealConstantOrMissing<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecRealConstantOrMissing<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect one byte per element.

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), baseValue + 0)) == (dh.get() + 1));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 1));
        dh.set(posNext);
        for (size_t n = 0; n < 255; n++) {
            EXPECT((posNext = c->encode(dh.get(), baseValue + n)) == (dh.get() + 1));
            dh.set(posNext);
        }
        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 1));
        dh.set(posNext);

        // Check we have the data we expect

        size_t nelem = 258;
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + nelem));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + nelem; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//                eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + nelem) == 0);
    }
}


CASE("Character strings are 8-byte sequences coerced into being treated as doubles") {

    // n.b. there are no missing values for CodecChars

    const size_t expectedHdrSize = 32;

    const char* expected_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                         // 0 = hasMissing
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // min = 987654321.9876 (big-endian) // UNUSED
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max = 987654321.9876 (big-endian) // UNUSED
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue = -2147483647        // UNUSED
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

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0 || j == 4) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);

            // n.b. Don't reverse the endianness of the string data.
            if (bigEndianOutput && j < 5)
                std::reverse(data.end()-len, data.end());
        }

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecChars<SameByteOrder>);
        } else {
            c.reset(new CodecChars<OtherByteOrder>);
        }

        c->missingValue(0.0);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(*reinterpret_cast<const double*>(expected_data[5]));
        c->gatherStats(*reinterpret_cast<const double*>(expected_data[6]));
        c->gatherStats(*reinterpret_cast<const double*>(expected_data[7]));
        c->gatherStats(*reinterpret_cast<const double*>(expected_data[8]));
        c->gatherStats(*reinterpret_cast<const double*>(expected_data[9]));

        EXPECT(!c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecChars<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecChars<OtherByteOrder>*>(c.get())->save(&dh);
        }
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data where we want it (ensuring that the data is written in appropriate
        // sized blocks.

        unsigned char* posNext;
        for(size_t n = 5; n < 10; n++) {
            EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(expected_data[n])))
                             == (dh.get() + 8));
            dh.set(posNext);
        }

        // Check we have the data we expect

        // n.b. We exclude the min/max/missing section of the header. This is not used for reading
        //      CodecChars (at the moment), and the existing codec does odd things. We don't want
        //      to code this behaviour into a test, as that would be weird.

        size_t dataSize = (8 * 5);

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + dataSize));
        EXPECT(::memcmp(&data[0], dh.getBuffer(), 4) == 0);
        EXPECT(::memcmp(&data[28], &dh.getBuffer()[28], expectedHdrSize + dataSize - 28) == 0);
    }
}


CASE("long floating point values can include the missing data value") {

    const uint64_t inf_bits = 0x7ff0000000000000;
    const uint64_t neg_inf_bits = 0xfff0000000000000;
    const uint64_t sig_nan_bits = 0xfffffffffffff77f;
    const uint64_t quiet_nan_bits = 0xffffffffffffff7f;

    const size_t expectedHdrSize = 28;
    double customMissingValue = 2222222222;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
        "\x00\x00\x00\x00\x00\x00\xf0\xff",  // minimum (-inf)
        "\x00\x00\x00\x00\x00\x00\xf0\x7f",  // maximum (+inf)
        "\x00\x00\xc0\x71\x8d\x8e\xe0\x41",  // missingValue = 2222222222

        // data to encode
        "\x00\x00\x00\x00\x00\x00\x00\x00",   // 0.0
        "\x53\xa4\x0c\x54\x34\x6f\x9d\x41",   // 123456789.0123456
        "\x9b\xe6\x57\xb7\x80\x65\x02\xc2",   // -9876543210.9876
        "\x00\x00\x00\x00\x00\x00\xf0\x7f",   // +inf
        "\x00\x00\x00\x00\x00\x00\xf0\xff",   // -inf
        "\x7f\xf7\xff\xff\xff\xff\xff\xff",   // NaN (signalling)
        "\x7f\xff\xff\xff\xff\xff\xff\xff",   // NaN (quiet)
        "\x00\x00\xc0\xff\xff\xff\xdf\xc1",   // -2147483647 (otherwise the missing value)
        "\x00\x00\xc0\x71\x8d\x8e\xe0\x41"    // missingValue = 2222222222
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecLongReal<SameByteOrder>);
        } else {
            c.reset(new CodecLongReal<OtherByteOrder>);
        }

        c->missingValue(customMissingValue);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(0.0);
        c->gatherStats(123456789.0123456);
        c->gatherStats(-9876543210.9876);
        double v = *reinterpret_cast<const double*>(&inf_bits);
        EXPECT(isinf(v));
        EXPECT(v > 0);
        c->gatherStats(v);
        v = *reinterpret_cast<const double*>(&neg_inf_bits);
        EXPECT(isinf(v));
        EXPECT(v < 0);
        c->gatherStats(v);
        v = *reinterpret_cast<const double*>(&sig_nan_bits);
        EXPECT(isnan(v));
        c->gatherStats(v);
        v = *reinterpret_cast<const double*>(&quiet_nan_bits);
        EXPECT(isnan(v));
        c->gatherStats(v);
        c->gatherStats(customMissingValue);
        c->gatherStats(-2147483647);

        // Detect that we have added a missing value
        EXPECT(c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecLongReal<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecLongReal<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 8 bytes per element

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), 0.0)) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), 123456789.0123456)) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), -9876543210.9876)) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(&inf_bits))) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(&neg_inf_bits))) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(&sig_nan_bits))) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(&quiet_nan_bits))) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), -2147483647)) == (dh.get() + 8));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 8));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (9 * 8);
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//                eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + data_size) == 0);
    }
}


CASE("short floating point values can include the missing data value") {

    // Use a curious, custom missingValue to show it is being used.
    // n.b. This cannot be represented with a float!

    const uint64_t inf_bits = 0x7ff0000000000000;
    const uint64_t neg_inf_bits = 0xfff0000000000000;
    const uint32_t sig_nan_bits = 0xffffbf7f;
    const uint32_t quiet_nan_bits = 0xffffff7f;

    const size_t expectedHdrSize = 28;
    double customMissingValue = -22222222222;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
        "\x00\x00\x00\x00\x00\x00\xf0\xff",  // minimum (-inf)
        "\x00\x00\x00\x00\x00\x00\xf0\x7f",  // maximum (+inf)
        "\x00\x00\x38\xce\x30\xb2\x14\xc2",  // missingValue = -22222222222

        // data to encode
        "\x00\x00\x00\x00",   // 0.0
        "\x12\xbf\x1f\x49",   // 654321.123
        "\x00\x00\x80\x00",   // Smallest available, internal missing value for short_real (1.17549435082229e-38)
        "\xff\xff\x7f\xff",   // Lowest available, internal missing value for short_real2 (-3.40282346638529e+38)
        "\x00\x00\x80\x7f",   // +inf
        "\x00\x00\x80\xff",   // -inf
        "\x7f\xbf\xff\xff",   // NaN (signalling)
        "\x7f\xff\xff\xff",   // NaN (quiet)
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 4; i++) {

        bool bigEndianOutput = (i % 2 == 0);
        bool secondCodec = (i > 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0 || j > 3) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        // Add the missing value!!!

        uint32_t mv = secondCodec ? 0xff7fffff : 0x00800000;
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&mv), reinterpret_cast<const unsigned char*>(&mv)+4);
        if (bigEndianOutput)
            std::reverse(data.end()-4, data.end());

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (secondCodec) {
                c.reset(new CodecShortReal2<SameByteOrder>);
            } else {
                c.reset(new CodecShortReal<SameByteOrder>);
            }
        } else {
            if (secondCodec) {
                c.reset(new CodecShortReal2<OtherByteOrder>);
            } else {
                c.reset(new CodecShortReal<OtherByteOrder>);
            }
        }

        c->missingValue(customMissingValue);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(0.0);
        c->gatherStats(654321.123);
        c->gatherStats(1.17549435082229e-38);
        c->gatherStats(-3.40282346638529e+38);
        double v = *reinterpret_cast<const double*>(&inf_bits);
        EXPECT(isinf(v));
        EXPECT(v > 0);
        c->gatherStats(v);
        v = *reinterpret_cast<const double*>(&neg_inf_bits);
        EXPECT(isinf(v));
        EXPECT(v < 0);
        c->gatherStats(v);
        v = static_cast<double>(*reinterpret_cast<const float*>(&sig_nan_bits));
        EXPECT(isnan(v));
        c->gatherStats(v);
        v = static_cast<double>(*reinterpret_cast<const float*>(&quiet_nan_bits));
        EXPECT(isnan(v));
        c->gatherStats(v);
        c->gatherStats(customMissingValue);

        // Detect that we have added a missing value
        EXPECT(c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (secondCodec) {
                static_cast<CodecShortReal2<SameByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecShortReal<SameByteOrder>*>(c.get())->save(&dh);
            }
        } else {
            if (secondCodec) {
                static_cast<CodecShortReal2<OtherByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecShortReal<OtherByteOrder>*>(c.get())->save(&dh);
            }
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 4 bytes per element

        // n.b. If we directly supply the float that is equivalent to the internal missing value, it
        //      is just treated as missing on read, without flagging hasMissing(). We do this here
        //      just to demonstrate. See ODB-367

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), 0.0)) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), 654321.123)) == (dh.get() + 4));
        dh.set(posNext);

        size_t offsetMissing = 0;
        if (secondCodec) {
            EXPECT((posNext = c->encode(dh.get(), 1.17549435082229e-38)) == (dh.get() + 4));
            dh.set(posNext);
            EXPECT_THROWS_AS(c->encode(dh.get(), -3.40282346638529e+38), eckit::AssertionFailed);
            offsetMissing = dh.position();
            dh.set(dh.get() + 4);
        } else {
            EXPECT_THROWS_AS(c->encode(dh.get(), 1.17549435082229e-38), eckit::AssertionFailed);
            offsetMissing = dh.position();
            dh.set(dh.get() + 4);
            EXPECT((posNext = c->encode(dh.get(), -3.40282346638529e+38)) == (dh.get() + 4));
            dh.set(posNext);
        }

        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(&inf_bits))) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(&neg_inf_bits))) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), static_cast<double>(*reinterpret_cast<const float*>(&sig_nan_bits)))) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), static_cast<double>(*reinterpret_cast<const float*>(&quiet_nan_bits)))) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 4));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (9 * 4);
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//                eckit::Log::info() << "******************************" << std::endl;
//        }

        // The missing values won't be encoded when they are hit, so skip them in the data test

        EXPECT(offsetMissing != 0);
        EXPECT(::memcmp(&data[0], dh.getBuffer(), offsetMissing) == 0);
        EXPECT(::memcmp(&data[0] + offsetMissing + 4,
                        dh.getBuffer() + offsetMissing + 4,
                        expectedHdrSize + data_size - offsetMissing - 4) == 0);
    }
}


CASE("32bit integers are as-is") {

    const size_t expectedHdrSize = 28;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
        "\x00\x00\x00\x00\x00\x00\xe0\xc1",  // minimum = -2147483648
        "\x00\x00\xc0\xff\xff\xff\xdf\x41",  // maximum = 2147483647
        "\x00\x00\x00\x1c\xaf\x7d\xaa\x41",  // missing value = 222222222

        // data to encode
        "\x00\x00\x00\x00",   // 0.0
        "\xff\xff\xff\xff",   // -1
        "\xff\xff\xff\x7f",   // 2147483647  == largest
        "\x00\x00\x00\x80",   // -2147483648 == smallest
        "\x8e\xd7\x3e\x0d",   // 222222222 == missingValue (unused by codec)
        "\x96\x28\x9c\xff"    // -6543210
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 2; i++) {

        bool bigEndianOutput = (i == 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0 || j > 3) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            c.reset(new CodecInt32<SameByteOrder>);
        } else {
            c.reset(new CodecInt32<OtherByteOrder>);
        }

        c->missingValue(222222222);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(0);
        c->gatherStats(-1);
        c->gatherStats(2147483647);
        c->gatherStats(-2147483648);
        EXPECT(!c->hasMissing());
        c->gatherStats(222222222);
        EXPECT(c->hasMissing());
        c->gatherStats(-6543210);

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            static_cast<CodecInt32<SameByteOrder>*>(c.get())->save(&dh);
        } else {
            static_cast<CodecInt32<OtherByteOrder>*>(c.get())->save(&dh);
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 4 bytes per element

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), 0)) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), -1)) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), 2147483647)) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), -2147483648)) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), 222222222)) == (dh.get() + 4));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), -6543210)) == (dh.get() + 4));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (6 * 4);
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//                eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + data_size) == 0);
    }
}


CASE("16bit integers are stored with an offset. This need not (strictly) be integral!!") {

    // n.b. we use a non-standard, non-integral minimum to demonstrate the offset behaviour.

    // Use a curious, custom missingValue to show it is being used.

    const double customMissingValue = 6.54565456545599971850917315786e-123;
    const double baseVal = -123.45;
    const size_t expectedHdrSize = 28;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
        "\xcd\xcc\xcc\xcc\xcc\xdc\x5e\xc0",  // minimum = -123.45
        "\x9a\x99\x99\x99\x71\xf0\xef\x40",  // maximum = -123.45 + 65535
        "\x04\x4f\xab\xa0\xe4\x4e\x91\x26",  // missing value = 6.54565456545599971850917315786e-123

        // data to encode
        "\x00\x00",   // 0.0
        "\xff\xff",   // 65535 and the missing value
        "\xff\x7f",   // 32767 (no negatives)
        "\x00\x80",   // 32768 (no negatives)
        "\x39\x30"    // 12345
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 4; i++) {

        bool bigEndianOutput = (i % 2 == 0);

        bool withMissing = (i > 1);

//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;
//        eckit::Log::info() << "ITERATION: " << i << std::endl;
//        eckit::Log::info() << "big endian: " << (bigEndianOutput ? "T" : "F") << std::endl;
//        eckit::Log::info() << "with missing: " << (withMissing ? "T" : "F") << std::endl;
//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : (j > 3) ? 2 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        // The missing value is odd. It should be handled properly for the Missing codec, but things are just mangled
        // if we do the direct codec.
        // See ODB-370 and ODB-371
        uint16_t mv = withMissing ? 0xffff : 0x007b;
        data.insert(data.end(), reinterpret_cast<const unsigned char*>(&mv), reinterpret_cast<const unsigned char*>(&mv) + 2);
        if (bigEndianOutput)
            std::reverse(data.end()-2, data.end());

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (withMissing) {
                c.reset(new CodecInt16Missing<SameByteOrder>);
            } else {
                c.reset(new CodecInt16<SameByteOrder>);
            }
        } else {
            if (withMissing) {
                c.reset(new CodecInt16Missing<OtherByteOrder>);
            } else {
                c.reset(new CodecInt16<OtherByteOrder>);
            }
        }

        c->missingValue(customMissingValue);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        c->gatherStats(baseVal + 0.0);
        c->gatherStats(baseVal + 65535); // n.b. This triggers a missingValue in Int16Missing. See ODB-369
        c->gatherStats(baseVal + 32767);
        c->gatherStats(baseVal + 32768);
        c->gatherStats(baseVal + 12345);
        EXPECT(!c->hasMissing());
        c->gatherStats(customMissingValue);
        EXPECT(c->hasMissing()); // See ODB-371

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (withMissing) {
                static_cast<CodecInt16<SameByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt16Missing<SameByteOrder>*>(c.get())->save(&dh);
            }
        } else {
            if (withMissing) {
                static_cast<CodecInt16<OtherByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt16Missing<OtherByteOrder>*>(c.get())->save(&dh);
            }
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 2 bytes per element

        // n.b. If we directly supply the value that is equivalent to the internal missing value, it
        //      is just treated as missing on read, without flagging hasMissing(). We do this here
        //      just to demonstrate. See ODB-369

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), baseVal + 0.0)) == (dh.get() + 2));
        dh.set(posNext);
        if (withMissing) {
            EXPECT_THROWS_AS(c->encode(dh.get(), baseVal + 65535), eckit::AssertionFailed);
            EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 2)); // Ensure data is the same
        } else {
            EXPECT((posNext = c->encode(dh.get(), baseVal + 65535)) == (dh.get() + 2));
        }
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), baseVal + 32767)) == (dh.get() + 2));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), baseVal + 32768)) == (dh.get() + 2));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), baseVal + 12345)) == (dh.get() + 2));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 2));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (6 * 2);
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//               eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + data_size) == 0);
    }
}


CASE("8bit integers are stored with an offset. This need not (strictly) be integral!!") {

    // n.b. we use a non-standard, non-integral minimum to demonstrate the offset behaviour.

    // Use a curious, custom missingValue to show it is being used.

    const double customMissingValue = 6.54565456545599971850917315786e-123;
    const double baseVal = -5000.5;
    const size_t expectedHdrSize = 28;

    const char* expected_data[] = {

        // Codec header
        "\x01\x00\x00\x00",                  // has missing value
        "\x00\x00\x00\x00\x80\x88\xb3\xc0",  // minimum = -5000.5
        "\x00\x00\x00\x00\x80\x89\xb2\xc0",  // maximum = -5000.5 + 255
        "\x04\x4f\xab\xa0\xe4\x4e\x91\x26",  // missing value = 6.54565456545599971850917315786e-123
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 4; i++) {

        bool bigEndianOutput = (i % 2 == 0);

        bool withMissing = (i > 1);

//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;
//        eckit::Log::info() << "ITERATION: " << i << std::endl;
//        eckit::Log::info() << "big endian: " << (bigEndianOutput ? "T" : "F") << std::endl;
//        eckit::Log::info() << "with missing: " << (withMissing ? "T" : "F") << std::endl;
//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len = (j == 0) ? 4 : 8;
            data.insert(data.end(), expected_data[j], expected_data[j] + len);
            if (bigEndianOutput)
                std::reverse(data.end()-len, data.end());
        }

        // Add all of the data values

        for (int n = 0; n < 256; n++) {
            data.push_back(static_cast<unsigned char>(n));
        }

        // n.b. we can end up with garbage for the missing value... in this case it will be 0x88...
        //      See ODB-371 ODB-370

        data.push_back(withMissing ? 0xff : 0x88);

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise the codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (withMissing) {
                c.reset(new CodecInt8Missing<SameByteOrder>);
            } else {
                c.reset(new CodecInt8<SameByteOrder>);
            }
        } else {
            if (withMissing) {
                c.reset(new CodecInt8Missing<OtherByteOrder>);
            } else {
                c.reset(new CodecInt8<OtherByteOrder>);
            }
        }

        c->missingValue(customMissingValue);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        // n.b. n == 255 will be silently promoted to missing if using Int8Missing. See ODB-369
        for (size_t n = 0; n < 256; n++) {
            c->gatherStats(baseVal + n);
        }
        EXPECT(!c->hasMissing());
        c->gatherStats(customMissingValue);
        EXPECT(c->hasMissing()); // See ODB-71

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (withMissing) {
                static_cast<CodecInt8<SameByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt8Missing<SameByteOrder>*>(c.get())->save(&dh);
            }
        } else {
            if (withMissing) {
                static_cast<CodecInt8<OtherByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt8Missing<OtherByteOrder>*>(c.get())->save(&dh);
            }
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 1 bytes per element

        // n.b. If we directly supply the value that is equivalent to the internal missing value, it
        //      is just treated as missing on read, without flagging hasMissing(). We do this here
        //      just to demonstrate. See ODB-369

        unsigned char* posNext;

        for (size_t n = 0; n < 255; n++) {
            EXPECT((posNext = c->encode(dh.get(), baseVal + n)) == (dh.get() + 1));
            dh.set(posNext);
        }

        if (withMissing) {
            EXPECT_THROWS_AS(c->encode(dh.get(), baseVal + 255), eckit::AssertionFailed);
            EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 1)); // Ensure data is the same
        } else {
            EXPECT((posNext = c->encode(dh.get(), baseVal + 255)) == (dh.get() + 1));
        }
        dh.set(posNext);

        EXPECT((posNext = c->encode(dh.get(), customMissingValue)) == (dh.get() + 1));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (257 * 1);
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//               eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + data_size) == 0);
    }
}


CASE("Character strings can be stored in a flat list, and indexed") {

    // n.b. no missing values

    const size_t expectedHdrSize = 144;

    const char* expected_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                         // 0 = hasMissing
        "\x6f\x70\x71\x72\x73\x74\x75\x76",         // min unspecified == "opqrstuv"
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue unspecified

        // How many strings are there in the table?
        "\x06\x00\x00\x00",

        // String data (prepended with lengths)
        // length, data, "cnt (discarded)", index

        // The order of these is a matter of implementation detail of the internal "hash table"
        // This is what happens to happen in current ODB-API

        "\x02\x00\x00\x00", "ab",           "\x00\x00\x00\x00", "\x00\x00\x00\x00", // This string is too short
        "\x06\x00\x00\x00", "ghijkl",       "\x00\x00\x00\x00", "\x01\x00\x00\x00",
        "\x08\x00\x00\x00", "mnopqrst",     "\x00\x00\x00\x00", "\x02\x00\x00\x00", // 8-byte length
        "\x08\x00\x00\x00", "uvwxyzab",     "\x00\x00\x00\x00", "\x03\x00\x00\x00", // n.b. truncated.
        "\x08\x00\x00\x00", "ghijklmn",     "\x00\x00\x00\x00", "\x04\x00\x00\x00",
        "\x08\x00\x00\x00", "opqrstuv",     "\x00\x00\x00\x00", "\x05\x00\x00\x00",
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 4; i++) {

        bool bigEndianOutput = (i % 2 == 0);

        bool bits16 = (i > 1);

//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;
//        eckit::Log::info() << "ITERATION: " << i << std::endl;
//        eckit::Log::info() << "big endian: " << (bigEndianOutput ? "T" : "F") << std::endl;
//        eckit::Log::info() << "16-bit: " << (bits16 ? "T" : "F") << std::endl;
//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len =
                    (j < 5) ? ((j == 0 || j == 4) ? 4 : 8)
                            : ((j+2) % 4 == 0 ? ::strlen(expected_data[j]) : 4);
            data.insert(data.end(), expected_data[j], expected_data[j] + len);

            // n.b. Don't reverse the endianness of the string data.
            if (bigEndianOutput && !((j > 5) && ((j+2) % 4 == 0)))
                std::reverse(data.end()-len, data.end());
        }

        // Which strings do we wish to encode (look at them in reverse. nb refers to index column)

        for (int n = 0; n < 6; n++) {
            if (bits16 && bigEndianOutput)
                data.push_back(0);
            data.push_back(static_cast<unsigned char>(n));
            if (bits16 && !bigEndianOutput)
                data.push_back(0);
        }

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (bits16) {
                c.reset(new CodecInt16String<SameByteOrder>);
            } else {
                c.reset(new CodecInt8String<SameByteOrder>);
            }
        } else {
            if (bits16) {
                c.reset(new CodecInt16String<OtherByteOrder>);
            } else {
                c.reset(new CodecInt8String<OtherByteOrder>);
            }
        }

        c->missingValue(0.0);
        EXPECT(!c->hasMissing());

        // Statistics in writing order

        const char* s1 = "ab"; // check that we can handle short strings!
        const char* s2 = "ghijkl";
        const char* s3 = "mnopqrst";
        const char* s4 = "uvwxyzabcdef"; // n.b. will be trucated to 8-bytes
        const char* s5 = "ghijklmn";
        const char* s6 = "opqrstuv";

        // n.b. these casts are a bit dubious in terms of memory access. May go beyond ends of s1, s2

        c->gatherStats(*reinterpret_cast<const double*>(s1));
        c->gatherStats(*reinterpret_cast<const double*>(s2));
        c->gatherStats(*reinterpret_cast<const double*>(s3));
        c->gatherStats(*reinterpret_cast<const double*>(s4));
        c->gatherStats(*reinterpret_cast<const double*>(s5));
        c->gatherStats(*reinterpret_cast<const double*>(s6));
        EXPECT(!c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (bits16) {
                static_cast<CodecInt16String<SameByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt8String<SameByteOrder>*>(c.get())->save(&dh);
            }
        } else {
            if (bits16) {
                static_cast<CodecInt16String<OtherByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt8String<OtherByteOrder>*>(c.get())->save(&dh);
            }
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 1 or 2 bytes per element

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s1))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s2))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s3))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s4))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s5))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s6))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (6 * (bits16 ? 2 : 1));
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//               eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + data_size) == 0);
    }
}


CASE("Character strings can be stored in a flat list, and indexed, and longer than 8 bytes") {

    // n.b. no missing values

    const size_t expectedHdrSize = 156;

    const char* expected_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                         // 0 = hasMissing
        "\x6f\x70\x71\x72\x73\x74\x75\x76",         // min unspecified == "opqrstuv"
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue unspecified

        // How many strings are there in the table?
        "\x06\x00\x00\x00",

        // String data (prepended with lengths)
        // length, data, "cnt (discarded)", index

        // The order of these is a matter of implementation detail of the internal "hash table"
        // This is what happens to happen in current ODB-API

        "\x02\x00\x00\x00", "ab",               "\x00\x00\x00\x00", "\x00\x00\x00\x00", // This string is too short
        "\x06\x00\x00\x00", "ghijkl",           "\x00\x00\x00\x00", "\x01\x00\x00\x00",
        "\x08\x00\x00\x00", "mnopqrst",         "\x00\x00\x00\x00", "\x02\x00\x00\x00", // 8-byte length
        "\x0c\x00\x00\x00", "uvwxyzabcdef",     "\x00\x00\x00\x00", "\x03\x00\x00\x00", // n.b. truncated.
        "\x10\x00\x00\x00", "ghijklmnopqrstuv", "\x00\x00\x00\x00", "\x04\x00\x00\x00",
        "\x08\x00\x00\x00", "opqrstuv",         "\x00\x00\x00\x00", "\x05\x00\x00\x00",
    };

    // Loop through endiannesses for the source data

    for (int i = 0; i < 4; i++) {

        bool bigEndianOutput = (i % 2 == 0);

        bool bits16 = (i > 1);

//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;
//        eckit::Log::info() << "ITERATION: " << i << std::endl;
//        eckit::Log::info() << "big endian: " << (bigEndianOutput ? "T" : "F") << std::endl;
//        eckit::Log::info() << "16-bit: " << (bits16 ? "T" : "F") << std::endl;
//        eckit::Log::info() << "---------------------------------------------------------" << std::endl;

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(expected_data) / sizeof(const char*); j++) {
            size_t len =
                    (j < 5) ? ((j == 0 || j == 4) ? 4 : 8)
                            : ((j+2) % 4 == 0 ? ::strlen(expected_data[j]) : 4);
            data.insert(data.end(), expected_data[j], expected_data[j] + len);

            // n.b. Don't reverse the endianness of the string data.
            if (bigEndianOutput && !((j > 5) && ((j+2) % 4 == 0)))
                std::reverse(data.end()-len, data.end());
        }

        // Which strings do we wish to encode (look at them in reverse. nb refers to index column)

        for (int n = 0; n < 6; n++) {
            if (bits16 && bigEndianOutput)
                data.push_back(0);
            data.push_back(static_cast<unsigned char>(n));
            if (bits16 && !bigEndianOutput)
                data.push_back(0);
        }

        MockWriteDataHandle dh; // Skip name of codec

        // Initialise codecs

        eckit::ScopedPtr<Codec> c;
        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (bits16) {
                c.reset(new CodecInt16String<SameByteOrder>);
            } else {
                c.reset(new CodecInt8String<SameByteOrder>);
            }
        } else {
            if (bits16) {
                c.reset(new CodecInt16String<OtherByteOrder>);
            } else {
                c.reset(new CodecInt8String<OtherByteOrder>);
            }
        }

        c->missingValue(0.0);
        EXPECT(!c->hasMissing());

        // Allow strings up to 16 bytes
        c->dataSizeDoubles(2);

        // Statistics in writing order

        const char* s1 = "ab"; // check that we can handle short strings!
        const char* s2 = "ghijkl";
        const char* s3 = "mnopqrst";
        const char* s4 = "uvwxyzabcdef"; // n.b. will NOT be trucated to 8-bytes
        const char* s5 = "ghijklmnopqrstuvwxyz"; // n.b. will be truncated to 16-bytes
        const char* s6 = "opqrstuv";

        // n.b. these casts are a bit dubious in terms of memory access. May go beyond ends of s1, s2

        c->gatherStats(*reinterpret_cast<const double*>(s1));
        c->gatherStats(*reinterpret_cast<const double*>(s2));
        c->gatherStats(*reinterpret_cast<const double*>(s3));
        c->gatherStats(*reinterpret_cast<const double*>(s4));
        c->gatherStats(*reinterpret_cast<const double*>(s5));
        c->gatherStats(*reinterpret_cast<const double*>(s6));
        EXPECT(!c->hasMissing());

        // Encode the header to the data stream

        if (bigEndianOutput == eckit::system::SystemInfo::isBigEndian()) {
            if (bits16) {
                static_cast<CodecInt16String<SameByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt8String<SameByteOrder>*>(c.get())->save(&dh);
            }
        } else {
            if (bits16) {
                static_cast<CodecInt16String<OtherByteOrder>*>(c.get())->save(&dh);
            } else {
                static_cast<CodecInt8String<OtherByteOrder>*>(c.get())->save(&dh);
            }
        }

        EXPECT(dh.position() == eckit::Offset(expectedHdrSize));

        // Encode the data to wherever we want it
        // Expect 1 or 2 bytes per element

        unsigned char* posNext;

        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s1))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s2))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s3))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s4))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s5))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);
        EXPECT((posNext = c->encode(dh.get(), *reinterpret_cast<const double*>(s6))) == (dh.get() + (bits16 ? 2 : 1)));
        dh.set(posNext);

        // Check we have the data we expect

        size_t data_size = (6 * (bits16 ? 2 : 1));
        EXPECT(dh.position() == eckit::Offset(expectedHdrSize + data_size));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < expectedHdrSize + data_size; n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh.getBuffer()[n]) << std::endl;
//            if (int(data[n]) != int(dh.getBuffer()[n]))
//               eckit::Log::info() << "******************************" << std::endl;
//        }

        EXPECT(::memcmp(&data[0], dh.getBuffer(), expectedHdrSize + data_size) == 0);
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    return run_tests(argc, argv);
}
