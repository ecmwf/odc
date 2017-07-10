#include "eckit/testing/Test.h"
#include "eckit/log/Timer.h"

#include "odb_api/Reader.h"

#include "TemporaryFiles.h"

#include <cstdint>

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("Test that the numeric limits are what we expect") {
    EXPECT(std::numeric_limits<std::int32_t>::max() == 2147483647);
    EXPECT(std::numeric_limits<std::int32_t>::min() == -2147483648);
}

CASE("Test reading min, max and intermediate values") {

    SETUP("An odb file containing min/max/intermediate values") {

        TemporaryODB tmpODB([](odb::Writer<>::iterator& writer) {

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
        });

        SECTION("Read the values back in") {

            odb::Reader oda(tmpODB.path());
            odb::Reader::iterator it = oda.begin();

            EXPECT((*it)[0] == std::numeric_limits<std::int32_t>::min());
            ++it;

            EXPECT((*it)[0] == std::numeric_limits<std::int32_t>::max());
            ++it;

            EXPECT((*it)[0] == std::numeric_limits<std::int32_t>::max() +
                               std::numeric_limits<std::int32_t>::min());
            ++it;
        }
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

