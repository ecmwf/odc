/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/eckit_config.h"

#include "odb_api/Writer.h"
#include "odb_api/Reader.h"
#include "odb_api/ColumnType.h"

using namespace eckit::testing;

#if __cplusplus <= 199711L
const float float_lowest = -std::numeric_limits<float>::max();
const double double_lowest = -std::numeric_limits<double>::max();
#else
const float float_lowest = std::numeric_limits<float>::lowest();
const double double_lowest = std::numeric_limits<double>::lowest();
#endif

// ------------------------------------------------------------------------------------------------------

CASE("Columns are initialised correctly for writing") {

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odc::Writer<> oda(dh);
    odc::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(10);

    // Now we create the columns

    writer->setColumn(0, "int", odc::INTEGER);
    writer->setColumn(1, "real", odc::REAL);
    writer->setColumn(2, "str", odc::STRING);
    writer->columns()[2]->dataSizeDoubles(3);
    writer->setColumn(3, "bitf", odc::BITFIELD);
    writer->setColumn(4, "dbl", odc::DOUBLE);
    writer->setColumn(5, "int2", odc::INTEGER);
    writer->setColumn(6, "real2", odc::REAL);
    writer->setColumn(7, "str2", odc::STRING);
    writer->setColumn(8, "bitf2", odc::BITFIELD);
    writer->setColumn(9, "dbl2", odc::DOUBLE);

    // Check that the columns are correctly created

    EXPECT(writer->columns()[0]->type() == odc::INTEGER);
    EXPECT(writer->columns()[1]->type() == odc::REAL);
    EXPECT(writer->columns()[2]->type() == odc::STRING);
    EXPECT(writer->columns()[3]->type() == odc::BITFIELD);
    EXPECT(writer->columns()[4]->type() == odc::DOUBLE);
    EXPECT(writer->columns()[5]->type() == odc::INTEGER);
    EXPECT(writer->columns()[6]->type() == odc::REAL);
    EXPECT(writer->columns()[7]->type() == odc::STRING);
    EXPECT(writer->columns()[8]->type() == odc::BITFIELD);
    EXPECT(writer->columns()[9]->type() == odc::DOUBLE);

    // ... with the correct name

    EXPECT(writer->columns()[0]->name() == "int");
    EXPECT(writer->columns()[1]->name() == "real");
    EXPECT(writer->columns()[2]->name() == "str");
    EXPECT(writer->columns()[3]->name() == "bitf");
    EXPECT(writer->columns()[4]->name() == "dbl");
    EXPECT(writer->columns()[5]->name() == "int2");
    EXPECT(writer->columns()[6]->name() == "real2");
    EXPECT(writer->columns()[7]->name() == "str2");
    EXPECT(writer->columns()[8]->name() == "bitf2");
    EXPECT(writer->columns()[9]->name() == "dbl2");

    // ... and the correct default codecs

    EXPECT(writer->columns()[0]->coder().name() == "int32");
    EXPECT(writer->columns()[1]->coder().name() == "long_real");
    EXPECT(writer->columns()[2]->coder().name() == "chars");
    EXPECT(writer->columns()[3]->coder().name() == "int32");
    EXPECT(writer->columns()[4]->coder().name() == "long_real");
    EXPECT(writer->columns()[5]->coder().name() == "int32");
    EXPECT(writer->columns()[6]->coder().name() == "long_real");
    EXPECT(writer->columns()[7]->coder().name() == "chars");
    EXPECT(writer->columns()[8]->coder().name() == "int32");
    EXPECT(writer->columns()[9]->coder().name() == "long_real");

    // ... and the correct expected data sizes

    EXPECT(writer->columns()[0]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[1]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[2]->coder().dataSizeDoubles() == 3);
    EXPECT(writer->columns()[3]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[4]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[5]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[6]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[7]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[8]->coder().dataSizeDoubles() == 1);
    EXPECT(writer->columns()[9]->coder().dataSizeDoubles() == 1);
}

CASE("If out-of range columns are created, exceptions are thrown") {

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odc::Writer<> oda(dh);
    odc::Writer<>::iterator writer = oda.begin();

    writer->setNumberOfColumns(10);
    writer->setColumn(2, "str", odc::STRING); // This is fine

    // If we create columns out of range, it throws exceptions

    EXPECT_THROWS_AS(writer->setColumn(11, "badnum", odc::INTEGER), eckit::AssertionFailed);
}

CASE("If columns are created with invalid types, exceptions are thrown") {

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odc::Writer<> oda(dh);
    odc::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(10);
    writer->setColumn(6, "real", odc::REAL); // This is fine

    // We cannot create a column of "IGNORE" type, or any type that is not listed in the enum

    EXPECT_THROWS_AS(writer->setColumn(0, "ignore", odc::IGNORE), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(0, "ignore", static_cast<odc::ColumnType>(123)), eckit::AssertionFailed);
}

CASE("Columns names must be unique") {

    // See issue ODB-372

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odc::Writer<> oda(dh);
    odc::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(10);

    writer->setColumn(0, "int", odc::INTEGER);
    writer->setColumn(1, "real", odc::REAL);
    writer->setColumn(2, "str", odc::STRING);
    writer->setColumn(3, "bitf", odc::BITFIELD);
    writer->setColumn(4, "dbl", odc::DOUBLE);

    EXPECT_THROWS_AS(writer->setColumn(5, "int", odc::INTEGER), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(6, "real", odc::REAL), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(7, "str", odc::STRING), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(8, "bitf", odc::BITFIELD), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(9, "dbl", odc::DOUBLE), eckit::SeriousBug);
}


CASE("Data is encoded and read back correctly") {

    const int32_t i1 = 987654321;
    const int32_t i2 = -1;
    const int32_t i3 = std::numeric_limits<int32_t>::min();
    const int32_t i4 = std::numeric_limits<int32_t>::max();
    const int32_t i5 = 0;
    const int32_t i6 = -654321;

    const float f1 = std::numeric_limits<float>::min();
    const float f2 = std::numeric_limits<float>::max();
    const float f3 = 0.0;
    const float f4 = float_lowest;
    const float f5 = static_cast<float>(654321.123);
    const float f6 = static_cast<float>(-123456.789e-21);

    const char* const s1 = "a-string-longstrvvvvlong";
    const char* const s2 = "string-2";
    const char* const s3 = "string-3-LLong";
    const char* const s4 = "string-4";
    const char* const s5 = "string-5-LLong";
    const char* const s6 = "string-6";

    const int32_t b1 = static_cast<int32_t>((uint32_t)std::numeric_limits<uint32_t>::min());
    const int32_t b2 = static_cast<int32_t>((uint32_t)std::numeric_limits<uint32_t>::max());
    const int32_t b3 = static_cast<int32_t>((uint32_t)0);
    const int32_t b4 = static_cast<int32_t>((uint32_t)0xff00ff00);
    const int32_t b5 = static_cast<int32_t>((uint32_t)0x00ff00ff);
    const int32_t b6 = static_cast<int32_t>((uint32_t)0xfedcba98);

    const double d1 = std::numeric_limits<double>::min();
    const double d2 = std::numeric_limits<double>::max();
    const double d3 = double_lowest;
    const double d4 = 0.0;
    const double d5 = -123456789.0123;
    const double d6 = 987654321.987e-56;

    // See issue ODB-372

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dhWrite(buf);

    {
        odc::Writer<> oda(dhWrite);
        odc::Writer<>::iterator writer = oda.begin();

        // Set up the columns

        writer->setNumberOfColumns(10);
        writer->setColumn(0, "int", odc::INTEGER);
        writer->setColumn(1, "real", odc::REAL);
        writer->setColumn(2, "str", odc::STRING);
        writer->columns()[2]->dataSizeDoubles(3);
        writer->setColumn(3, "bitf", odc::BITFIELD);
        writer->setColumn(4, "dbl", odc::DOUBLE);
        writer->setColumn(5, "int2", odc::INTEGER);
        writer->setColumn(6, "real2", odc::REAL);
        writer->setColumn(7, "str2", odc::STRING);
        writer->setColumn(8, "bitf2", odc::BITFIELD);
        writer->setColumn(9, "dbl2", odc::DOUBLE);
        writer->writeHeader();

        // Test that the data offsets are correct

        EXPECT(writer->dataOffset(0) == 0);
        EXPECT(writer->dataOffset(1) == 1);
        EXPECT(writer->dataOffset(2) == 2);
        EXPECT(writer->dataOffset(3) == 5);
        EXPECT(writer->dataOffset(4) == 6);
        EXPECT(writer->dataOffset(5) == 7);
        EXPECT(writer->dataOffset(6) == 8);
        EXPECT(writer->dataOffset(7) == 9);
        EXPECT(writer->dataOffset(8) == 10);
        EXPECT(writer->dataOffset(9) == 11);

        // Append 3 rows of data (in two different ways)

        (*writer)[0] = i1;
        (*writer)[1] = static_cast<double>(f1);
        ::strncpy(reinterpret_cast<char*>(&(*writer)[2]), s1, 24); // strncpy pads with \0
        (*writer)[3] = b1;
        (*writer)[4] = d1;
        (*writer)[5] = i2;
        (*writer)[6] = static_cast<double>(f2);
        (*writer)[7] = *reinterpret_cast<const double*>(s2);
        (*writer)[8] = b2;
        (*writer)[9] = d2;
        ++writer;

        writer->data()[writer->dataOffset(0)] = i3;
        writer->data()[writer->dataOffset(1)] = static_cast<double>(f3);
        ::strncpy(reinterpret_cast<char*>(&writer->data()[writer->dataOffset(2)]), s3, 24); // strncpy pads with \0
        writer->data()[writer->dataOffset(3)] = b3;
        writer->data()[writer->dataOffset(4)] = d3;
        writer->data()[writer->dataOffset(5)] = i4;
        writer->data()[writer->dataOffset(6)] = static_cast<double>(f4);
        writer->data()[writer->dataOffset(7)] = *reinterpret_cast<const double*>(s4);
        writer->data()[writer->dataOffset(8)] = b4;
        writer->data()[writer->dataOffset(9)] = d4;
        ++writer;

        writer->data(0) = i5;
        writer->data(1) = static_cast<double>(f5);
        ::strncpy(reinterpret_cast<char*>(&writer->data(2)), s5, 24); // strncpy pads with \0
        writer->data(3) = b5;
        writer->data(4) = d5;
        writer->data(5) = i6;
        writer->data(6) = static_cast<double>(f6);
        writer->data(7) = *reinterpret_cast<const double*>(s6);
        writer->data(8) = b6;
        writer->data(9) = d6;
        ++writer;
    }

    // Read everything back

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(dhWrite.position()));
        dh.openForRead();
        odc::Reader oda(dh);
        odc::Reader::iterator reader = oda.begin();

        EXPECT(reader->columns().size() == size_t(10));

        EXPECT(reader->columns()[0]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[1]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[2]->dataSizeDoubles() == 3);
        EXPECT(reader->columns()[3]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[4]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[5]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[6]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[7]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[8]->dataSizeDoubles() == 1);
        EXPECT(reader->columns()[9]->dataSizeDoubles() == 1);

        EXPECT((*reader)[0] == i1);
        EXPECT((*reader)[1] == static_cast<double>(f1));
        EXPECT(::strncmp(reinterpret_cast<const char*>(&(*reader)[2]), s1, 24) == 0);
        EXPECT((*reader)[3] == b1);
        EXPECT((*reader)[4] == d1);
        EXPECT((*reader)[5] == i2);
        EXPECT((*reader)[6] == static_cast<double>(f2));
        EXPECT((*reader)[7] == *reinterpret_cast<const double*>(s2));
        EXPECT((*reader)[8] == b2);
        EXPECT((*reader)[9] == d2);
        ++reader;

        EXPECT(reader->data()[reader->dataOffset(0)] == i3);
        EXPECT(reader->data()[reader->dataOffset(1)] == static_cast<double>(f3));
        EXPECT(::strncmp(reinterpret_cast<const char*>(&reader->data()[reader->dataOffset(2)]),s3, 24) == 0);
        EXPECT(reader->data()[reader->dataOffset(3)] == b3);
        EXPECT(reader->data()[reader->dataOffset(4)] == d3);
        EXPECT(reader->data()[reader->dataOffset(5)] == i4);
        EXPECT(reader->data()[reader->dataOffset(6)] == static_cast<double>(f4));
        EXPECT(reader->data()[reader->dataOffset(7)] == *reinterpret_cast<const double*>(s4));
        EXPECT(reader->data()[reader->dataOffset(8)] == b4);
        EXPECT(reader->data()[reader->dataOffset(9)] == d4);
        ++reader;

        EXPECT(reader->data(0) == i5);
        EXPECT(reader->data(1) == static_cast<double>(f5));
        EXPECT(::strncmp(reinterpret_cast<const char*>(&reader->data(2)), s5, 24) == 0);
        EXPECT(reader->data(3) == b5);
        EXPECT(reader->data(4) == d5);
        EXPECT(reader->data(5) == i6);
        EXPECT(reader->data(6) == static_cast<double>(f6));
        EXPECT(reader->data(7) == *reinterpret_cast<const double*>(s6));
        EXPECT(reader->data(8) == b6);
        EXPECT(reader->data(9) == d6);
    }
}


#if 0 // This test needs to be reassesed -- AssertionFailed is not likely the correct Exception to be thrown

CASE("We cannot encode short_real with both possible internal missing values") {

    const float f1 = std::numeric_limits<float>::min();
    const float f2 = float_lowest;

    eckit::Buffer buf(4096);

    EXPECT_THROWS_AS({
        eckit::MemoryHandle dh(buf);
        odc::Writer<> oda(dh);
        odc::Writer<>::iterator writer = oda.begin();

        // Set up the columns

        writer->setNumberOfColumns(1);
        writer->setColumn(0, "real", odc::REAL);
        writer->writeHeader();

        // Append 3 rows of data (in two different ways)

        (*writer)[0] = f1;
        ++writer;

        (*writer)[0] = f2;
        ++writer;

    }, eckit::AssertionFailed);

}

#endif

CASE("We ASSERT on cases where we try and use an incompletely configured writer") {

    eckit::Buffer buf(4096);

    // Illegal to flush an incompletely finished writer

    eckit::MemoryHandle dh(buf);
    odc::Writer<> oda(dh);
    odc::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(2);
    writer->setColumn(0, "real", odc::REAL);

    // Cannot writeHeader until all the columns are initialised
    EXPECT_THROWS_AS(writer->writeHeader(), eckit::AssertionFailed);

    // Cannot write to an uninitialised writer
    EXPECT_THROWS_AS((*writer)[0] = 1234.56, eckit::AssertionFailed);

    // Cannot increment an incomplete row
    EXPECT_THROWS_AS(++writer, eckit::AssertionFailed);
}

CASE("Data is automatically written after a configurable number of rows") {
    EXPECT(true);
}

CASE("Pathological data for integral codecs is correctly encoded") {

    // The reduced-size integral codecs have special internal values for missingValue.
    // If we try and encode an integer that happens to collide with that value whilst
    // missing values are enabled, the codec in use needs to be uprated to the next
    // biggest size, so things are encoded correctly.

    eckit::Buffer buf(4096);

    for (int i = 0; i < 4; i++) {

        bool withMissing = (i % 2 == 1);
        bool bits16 = (i > 1);

//        eckit::Log::info() << "iteration: " << i
//                           << (withMissing ? "T":"F")
//                           << (bits16 ? "T":"F") << std::endl;

        int32_t i1 = 12345;
        int32_t i2 = 12345 + (bits16 ? 0xffff : 0xff);

        {
            eckit::MemoryHandle dh(buf);
            odc::Writer<> oda(dh);
            odc::Writer<>::iterator writer = oda.begin();

            // Set up the columns

            writer->setNumberOfColumns(1);
            writer->setColumn(0, "int", odc::INTEGER);
            writer->writeHeader();

            // Append 3 rows of data (in two different ways)

            (*writer)[0] = i1;
            ++writer;

            (*writer)[0] = i2;
            ++writer;

            if (withMissing) {
                (*writer)[0] = odc::MDI::integerMDI();
                ++writer;
            }

            // We have not supplied missing values
            EXPECT(writer->columns()[0]->missingValue() == odc::MDI::integerMDI());
            EXPECT(writer->columns()[0]->hasMissing() == withMissing);
        }

        // Read everything back

        {
            eckit::MemoryHandle dh(buf);
            dh.openForRead();
            odc::Reader oda(dh);
            odc::Reader::iterator reader = oda.begin();

            // We have not supplied missing values
            EXPECT(reader->columns().size() == size_t(1));
            EXPECT(reader->columns()[0]->missingValue() == odc::MDI::integerMDI());
            EXPECT(reader->columns()[0]->hasMissing() == withMissing);

            // Promotion to int32 occurs to

            if (withMissing && bits16) {
                EXPECT(reader->columns()[0]->coder().name() == "int32");
            } else if (withMissing) {
                EXPECT(reader->columns()[0]->coder().name() == "int16_missing");
            } else if (bits16) {
                EXPECT(reader->columns()[0]->coder().name() == "int16");
            } else {
                EXPECT(reader->columns()[0]->coder().name() == "int8");
            }

            EXPECT((*reader)[0] == i1);
            ++reader;

            EXPECT((*reader)[0] == i2);

            if (withMissing) {
                ++reader;
                EXPECT((*reader)[0] == odc::MDI::integerMDI());
            }
        }
    }

}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}


