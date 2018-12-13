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
// i) Make them templated on the stream/datahandle directly
// ii) Construct them with a specific data handle/stream
// iii) Why are we casting data handles via a void* ???

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


CASE("Character strings can be stored in a flat list, and indexed") {

    // n.b. no missing values

    const char* source_data[] = {

        // Codec header
        "\x00\x00\x00\x00",                         // 0 = hasMissing
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // min unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // max unspecified
        "\x00\x00\x00\x00\x00\x00\x00\x00",         // missingValue unspecified

        // How many strings are there in the table?
        "\x06\x00\x00\x00",

        // String data (prepended with lengths)
        // length, data, "cnt (discarded)", index

        "\x08\x00\x00\x00", "ghijklmn",     "\x00\x00\x00\x00", "\x00\x00\x00\x00",
        "\x0c\x00\x00\x00", "uvwxyzabcdef", "\x00\x00\x00\x00", "\x01\x00\x00\x00", // too long
        "\x08\x00\x00\x00", "opqrstuv",     "\x00\x00\x00\x00", "\x02\x00\x00\x00",
        "\x02\x00\x00\x00", "ab",           "\x00\x00\x00\x00", "\x03\x00\x00\x00", // This string is too short
        "\x06\x00\x00\x00", "ghijkl",       "\x00\x00\x00\x00", "\x04\x00\x00\x00",
        "\x08\x00\x00\x00", "mnopqrst",     "\x00\x00\x00\x00", "\x05\x00\x00\x00", // 8-byte length
    };

    // Loop throumgh endiannesses for the source data

    for (int i = 0; i < 4; i++) {

        bool bigEndianSource = (i % 2 == 0);

        bool bits16 = (i > 1);

        std::vector<unsigned char> data;

        for (size_t j = 0; j < sizeof(source_data) / sizeof(const char*); j++) {
            size_t len =
                    (j < 5) ? ((j == 0 || j == 4) ? 4 : 8)
                            : ((j+2) % 4 == 0 ? ::strlen(source_data[j]) : 4);
            data.insert(data.end(), source_data[j], source_data[j] + len);

            // n.b. Don't reverse the endianness of the string data.
            if (bigEndianSource && !((j > 5) && ((j+2) % 4 == 0)))
                std::reverse(data.end()-len, data.end());
        }

        // Which strings do we wish to decode (look at them in reverse. nb refers to index column)

        for (int n = 5; n >= 0; n--) {
            if (bits16 && bigEndianSource)
                data.push_back(0);
            data.push_back(static_cast<unsigned char>(n));
            if (bits16 && !bigEndianSource)
                data.push_back(0);
        }

        // Construct codec directly, and decode the header

        MockReadDataHandle dh(data); // Skip name of codec

        eckit::ScopedPtr<Codec> c;
        if (bigEndianSource == eckit::system::SystemInfo::isBigEndian()) {
            if (bits16) {
                c.reset(new CodecInt16String<SameByteOrder>);
                static_cast<CodecInt16String<SameByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecInt8String<SameByteOrder>);
                static_cast<CodecInt8String<SameByteOrder>*>(c.get())->load(&dh);
            }
        } else {
            if (bits16) {
                c.reset(new CodecInt16String<OtherByteOrder>);
                static_cast<CodecInt16String<OtherByteOrder>*>(c.get())->load(&dh);
            } else {
                c.reset(new CodecInt8String<OtherByteOrder>);
                static_cast<CodecInt8String<OtherByteOrder>*>(c.get())->load(&dh);
            }
        }

        EXPECT(dh.position() == eckit::Offset(148));

        // Now re-encode the codec header, and check that we get what we started with!

        MockWriteDataHandle dh_write;

        if (bigEndianSource == eckit::system::SystemInfo::isBigEndian()) {
            if (bits16) {
                static_cast<CodecInt16String<SameByteOrder>*>(c.get())->save(&dh_write);
            } else {
                static_cast<CodecInt8String<SameByteOrder>*>(c.get())->save(&dh_write);
            }
        } else {
            if (bits16) {
                static_cast<CodecInt16String<OtherByteOrder>*>(c.get())->save(&dh_write);
            } else {
                static_cast<CodecInt8String<OtherByteOrder>*>(c.get())->save(&dh_write);
            }
        }

        // Check that the data is the same both times!

        EXPECT(dh_write.position() == eckit::Offset(148));

//        eckit::Log::info() << "DATA: " << std::endl;
//        for (size_t n = 0; n < data.size(); n++) {
//            eckit::Log::info() << std::hex << int(data[n]) << " " << int(dh_write.getBuffer()[n]) << std::endl;
//           if (int(data[n]) != int(dh_write.getBuffer()[n]))
//               eckit::Log::info() << "******************************" << std::endl;
//        }

        // The header should be correctly re-encoded.
        EXPECT(::memcmp(dh_write.getBuffer(), &data[0], 148) == 0);

        // We haven't encoded the data itself
        for (size_t i = 148; i < 154; i++) {
            EXPECT(dh_write.getBuffer()[i] == 0);
        }
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    return run_tests(argc, argv);
}
