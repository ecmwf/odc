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

#include "odb_api/Writer.h"
#include "odb_api/ColumnType.h"

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("Data of all available types is encoded correctly") {

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
}

CASE("If out-of range columns are created, exceptions are thrown") {

    eckit::Buffer buf(4096);
    eckit::MemoryHandle dh(buf);

    odb::Writer<> oda(dh);
    odb::Writer<>::iterator writer = oda.begin();

    writer->setNumberOfColumns(10);
    writer->setColumn(2, "str", odb::STRING); // This is fine

    // If we create columns out of range, it throws exceptions

    std::cout << std::endl;
    std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv EXPECTED EXCEPTIONS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" << std::endl;
    EXPECT_THROWS_AS(writer->setColumn(11, "badnum", odb::INTEGER), eckit::AssertionFailed);
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DONE EXCEPTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    std::cout << std::endl;
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

    std::cout << std::endl;
    std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv EXPECTED EXCEPTIONS vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" << std::endl;
    EXPECT_THROWS_AS(writer->setColumn(0, "ignore", odb::IGNORE), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(0, "ignore", static_cast<odb::ColumnType>(123)), eckit::AssertionFailed);
    std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DONE EXCEPTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
    std::cout << std::endl;
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

    EXPECT_THROWS_AS(writer->setColumn(5, "int", odb::INTEGER), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(6, "real", odb::REAL), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(7, "str", odb::STRING), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(8, "bitf", odb::BITFIELD), eckit::AssertionFailed);
    EXPECT_THROWS_AS(writer->setColumn(9, "dbl", odb::DOUBLE), eckit::AssertionFailed);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}


