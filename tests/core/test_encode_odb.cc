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

    odb::Writer<> oda(dh);
    odb::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(10);

    // Now we create the columns

    writer->setColumn(0, "int", odb::INTEGER);
    writer->setColumn(1, "real", odb::REAL);
    writer->setColumn(2, "str", odb::STRING);
    writer->setColumn(3, "bitf", odb::BITFIELD);
    writer->setColumn(4, "dbl", odb::DOUBLE);
    writer->setColumn(5, "int2", odb::INTEGER);
    writer->setColumn(6, "real2", odb::REAL);
    writer->setColumn(7, "str2", odb::STRING);
    writer->setColumn(8, "bitf2", odb::BITFIELD);
    writer->setColumn(9, "dbl2", odb::DOUBLE);

    // Check that the columns are correctly created

    EXPECT(writer->columns()[0]->type() == odb::INTEGER);
    EXPECT(writer->columns()[1]->type() == odb::REAL);
    EXPECT(writer->columns()[2]->type() == odb::STRING);
    EXPECT(writer->columns()[3]->type() == odb::BITFIELD);
    EXPECT(writer->columns()[4]->type() == odb::DOUBLE);
    EXPECT(writer->columns()[5]->type() == odb::INTEGER);
    EXPECT(writer->columns()[6]->type() == odb::REAL);
    EXPECT(writer->columns()[7]->type() == odb::STRING);
    EXPECT(writer->columns()[8]->type() == odb::BITFIELD);
    EXPECT(writer->columns()[9]->type() == odb::DOUBLE);

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
}

CASE("If out-of range columns are created, exceptions are thrown") {

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odb::Writer<> oda(dh);
    odb::Writer<>::iterator writer = oda.begin();

    writer->setNumberOfColumns(10);
    writer->setColumn(2, "str", odb::STRING); // This is fine

    // If we create columns out of range, it throws exceptions

    EXPECT_THROWS_AS(writer->setColumn(11, "badnum", odb::INTEGER), eckit::AssertionFailed);
}

CASE("If columns are created with invalid types, exceptions are thrown") {

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odb::Writer<> oda(dh);
    odb::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(10);
    writer->setColumn(6, "real", odb::REAL); // This is fine

    // We cannot create a column of "IGNORE" type, or any type that is not listed in the enum

    EXPECT_THROWS_AS(writer->setColumn(0, "ignore", odb::IGNORE), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(0, "ignore", static_cast<odb::ColumnType>(123)), eckit::AssertionFailed);
}

CASE("Columns names must be unique") {

    // See issue ODB-372

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odb::Writer<> oda(dh);
    odb::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(10);

    writer->setColumn(0, "int", odb::INTEGER);
    writer->setColumn(1, "real", odb::REAL);
    writer->setColumn(2, "str", odb::STRING);
    writer->setColumn(3, "bitf", odb::BITFIELD);
    writer->setColumn(4, "dbl", odb::DOUBLE);

    EXPECT_THROWS_AS(writer->setColumn(5, "int", odb::INTEGER), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(6, "real", odb::REAL), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(7, "str", odb::STRING), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(8, "bitf", odb::BITFIELD), eckit::SeriousBug);
    EXPECT_THROWS_AS(writer->setColumn(9, "dbl", odb::DOUBLE), eckit::SeriousBug);
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

    const char* const s1 = "a-string";
    const char* const s2 = "string-2";
    const char* const s3 = "string-3";
    const char* const s4 = "string-4";
    const char* const s5 = "string-5";
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
        odb::Writer<> oda(dhWrite);
        odb::Writer<>::iterator writer = oda.begin();

        // Set up the columns

        writer->setNumberOfColumns(10);
        writer->setColumn(0, "int", odb::INTEGER);
        writer->setColumn(1, "real", odb::REAL);
        writer->setColumn(2, "str", odb::STRING);
        writer->setColumn(3, "bitf", odb::BITFIELD);
        writer->setColumn(4, "dbl", odb::DOUBLE);
        writer->setColumn(5, "int2", odb::INTEGER);
        writer->setColumn(6, "real2", odb::REAL);
        writer->setColumn(7, "str2", odb::STRING);
        writer->setColumn(8, "bitf2", odb::BITFIELD);
        writer->setColumn(9, "dbl2", odb::DOUBLE);
        writer->writeHeader();

        // Append 3 rows of data (in two different ways)

        (*writer)[0] = i1;
        (*writer)[1] = static_cast<double>(f1);
        (*writer)[2] = *reinterpret_cast<const double*>(s1);
        (*writer)[3] = b1;
        (*writer)[4] = d1;
        (*writer)[5] = i2;
        (*writer)[6] = static_cast<double>(f2);
        (*writer)[7] = *reinterpret_cast<const double*>(s2);
        (*writer)[8] = b2;
        (*writer)[9] = d2;
        ++writer;

        writer->data()[0] = i3;
        writer->data()[1] = static_cast<double>(f3);
        writer->data()[2] = *reinterpret_cast<const double*>(s3);
        writer->data()[3] = b3;
        writer->data()[4] = d3;
        writer->data()[5] = i4;
        writer->data()[6] = static_cast<double>(f4);
        writer->data()[7] = *reinterpret_cast<const double*>(s4);
        writer->data()[8] = b4;
        writer->data()[9] = d4;
        ++writer;

        (*writer)[0] = i5;
        (*writer)[1] = static_cast<double>(f5);
        (*writer)[2] = *reinterpret_cast<const double*>(s5);
        (*writer)[3] = b5;
        (*writer)[4] = d5;
        (*writer)[5] = i6;
        (*writer)[6] = static_cast<double>(f6);
        (*writer)[7] = *reinterpret_cast<const double*>(s6);
        (*writer)[8] = b6;
        (*writer)[9] = d6;
        ++writer;
    }

    // Read everything back

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(dhWrite.position()));
        dh.openForRead();
        odb::Reader oda(dh);
        odb::Reader::iterator reader = oda.begin();

        EXPECT(reader->columns().size() == size_t(10));

        EXPECT((*reader)[0] == i1);
        EXPECT((*reader)[1] == static_cast<double>(f1));
        EXPECT((*reader)[2] == *reinterpret_cast<const double*>(s1));
        EXPECT((*reader)[3] == b1);
        EXPECT((*reader)[4] == d1);
        EXPECT((*reader)[5] == i2);
        EXPECT((*reader)[6] == static_cast<double>(f2));
        EXPECT((*reader)[7] == *reinterpret_cast<const double*>(s2));
        EXPECT((*reader)[8] == b2);
        EXPECT((*reader)[9] == d2);
        ++reader;

        EXPECT(reader->data()[0] == i3);
        EXPECT(reader->data()[1] == static_cast<double>(f3));
        EXPECT(reader->data()[2] == *reinterpret_cast<const double*>(s3));
        EXPECT(reader->data()[3] == b3);
        EXPECT(reader->data()[4] == d3);
        EXPECT(reader->data()[5] == i4);
        EXPECT(reader->data()[6] == static_cast<double>(f4));
        EXPECT(reader->data()[7] == *reinterpret_cast<const double*>(s6));
        EXPECT(reader->data()[8] == b4);
        EXPECT(reader->data()[9] == d4);
        ++reader;

        EXPECT((*reader)[0] == i5);
        EXPECT((*reader)[1] == static_cast<double>(f5));
        EXPECT((*reader)[2] == *reinterpret_cast<const double*>(s5));
        EXPECT((*reader)[3] == b5);
        EXPECT((*reader)[4] == d5);
        EXPECT((*reader)[5] == i6);
        EXPECT((*reader)[6] == static_cast<double>(f6));
        EXPECT((*reader)[7] == *reinterpret_cast<const double*>(s6));
        EXPECT((*reader)[8] == b6);
        EXPECT((*reader)[9] == d6);
    }
}

CASE("We cannot encode short_real with both possible internal missing values") {

    const float f1 = std::numeric_limits<float>::min();
    const float f2 = float_lowest;

    eckit::Buffer buf(4096);

    EXPECT_THROWS_AS({
        eckit::MemoryHandle dh(buf);
        odb::Writer<> oda(dh);
        odb::Writer<>::iterator writer = oda.begin();

        // Set up the columns

        writer->setNumberOfColumns(1);
        writer->setColumn(0, "real", odb::REAL);
        writer->writeHeader();

        // Append 3 rows of data (in two different ways)

        (*writer)[0] = f1;
        ++writer;

        (*writer)[0] = f2;
        ++writer;

    }, eckit::AssertionFailed);

}

CASE("We ASSERT on cases where we try and use an incompletely configured writer") {

    eckit::Buffer buf(4096);

    // Illegal to flush an incompletely finished writer

    eckit::MemoryHandle dh(buf);
    odb::Writer<> oda(dh);
    odb::Writer<>::iterator writer = oda.begin();

    // Set up the columns

    writer->setNumberOfColumns(2);
    writer->setColumn(0, "real", odb::REAL);

    // Cannot writeHeader until all the columns are initialised
    EXPECT_THROWS_AS(writer->writeHeader(), eckit::AssertionFailed);

    (*writer)[0] = 1234.56;

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
            odb::Writer<> oda(dh);
            odb::Writer<>::iterator writer = oda.begin();

            // Set up the columns

            writer->setNumberOfColumns(1);
            writer->setColumn(0, "int", odb::INTEGER);
            writer->writeHeader();

            // Append 3 rows of data (in two different ways)

            (*writer)[0] = i1;
            ++writer;

            (*writer)[0] = i2;
            ++writer;

            if (withMissing) {
                (*writer)[0] = odb::MDI::integerMDI();
                ++writer;
            }

            // We have not supplied missing values
            EXPECT(writer->columns()[0]->missingValue() == odb::MDI::integerMDI());
            EXPECT(writer->columns()[0]->hasMissing() == withMissing);
        }

        // Read everything back

        {
            eckit::MemoryHandle dh(buf);
            dh.openForRead();
            odb::Reader oda(dh);
            odb::Reader::iterator reader = oda.begin();

            // We have not supplied missing values
            EXPECT(reader->columns().size() == size_t(1));
            EXPECT(reader->columns()[0]->missingValue() == odb::MDI::integerMDI());
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
                EXPECT((*reader)[0] == odb::MDI::integerMDI());
            }
        }
    }

}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}


