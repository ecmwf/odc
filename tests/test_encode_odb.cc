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


