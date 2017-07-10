#include "eckit/testing/Test.h"
#include "eckit/log/Timer.h"

#include "odb_api/Writer.h"
#include "odb_api/Reader.h"

#include <cstdint>

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

// A fixture that writes an ODB file in a unique temporary file, and destroys it when done.

struct MinMaxODBWriter {

    MinMaxODBWriter() : path_("_temporary_testminmax.odb") {

        eckit::Timer t("Writing testminmax.odb");
        odb::Writer<> oda(path_);

        odb::Writer<>::iterator row = oda.begin();
        row->setNumberOfColumns(2);

        row->setColumn(0, "intcol", odb::INTEGER);
        row->setColumn(1, "realcol", odb::REAL);

        row->writeHeader();

        (*row)[0] = std::numeric_limits<int32_t>::min();
        (*row)[1] = 1;
        ++row;

        (*row)[0] = std::numeric_limits<int32_t>::max();
        (*row)[1] = 1;
        ++row;

        (*row)[0] = std::numeric_limits<int32_t>::min() + std::numeric_limits<int32_t>::max();
        (*row)[1] = 1;
        ++row;
    }

    ~MinMaxODBWriter() {
        path_.unlink();
    }

    const eckit::PathName& path() const { return path_; }

private: // method
    eckit::PathName path_;
};

// ------------------------------------------------------------------------------------------------------

const Test specification[] = {

    CASE("Test that the numeric limits are what we expect") {
        EXPECT(std::numeric_limits<std::int32_t>::max() == 2147483647);
        EXPECT(std::numeric_limits<std::int32_t>::min() == -2147483648);
    },

    CASE("Test reading min, max and intermediate values") {

        SETUP("An odb file containing min/max/intermediate values") {

            MinMaxODBWriter writer;

            SECTION("Read the values back in") {

                odb::Reader oda(writer.path());
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
};

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(specification, argc, argv);
}

