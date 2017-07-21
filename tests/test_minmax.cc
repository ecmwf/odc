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

#include "odb_api/Reader.h"

#include "TemporaryFiles.h"

#include <stdint.h>

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("Test that the numeric limits are what we expect") {
    EXPECT(std::numeric_limits<int32_t>::max() == 2147483647);
    EXPECT(std::numeric_limits<int32_t>::min() == -2147483648);
}

CASE("Test reading min, max and intermediate values") {

    SETUP("An odb file containing min/max/intermediate values") {

        class TemporaryMinMaxODB : public TemporaryFile {
        public:
            TemporaryMinMaxODB() {
                odb::Writer<> oda(path());
                odb::Writer<>::iterator writer = oda.begin();

                writer->setNumberOfColumns(2);
                writer->setColumn(0, "intcol", odb::INTEGER);
                writer->setColumn(1, "realcol", odb::REAL);
                writer->writeHeader();

                (*writer)[0] = std::numeric_limits<int32_t>::min();
                (*writer)[1] = 1;
                ++writer;

                (*writer)[0] = std::numeric_limits<int32_t>::max();
                (*writer)[1] = 1;
                ++writer;

                (*writer)[0] = std::numeric_limits<int32_t>::min() + std::numeric_limits<int32_t>::max();
                (*writer)[1] = 1;
                ++writer;
            }
        };

        TemporaryMinMaxODB tmpODB;

        SECTION("Read the values back in") {

            odb::Reader oda(tmpODB.path());
            odb::Reader::iterator it = oda.begin();

            EXPECT((*it)[0] == std::numeric_limits<int32_t>::min());
            ++it;

            EXPECT((*it)[0] == std::numeric_limits<int32_t>::max());
            ++it;

            EXPECT((*it)[0] == std::numeric_limits<int32_t>::max() +
                               std::numeric_limits<int32_t>::min());
            ++it;
        }
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

