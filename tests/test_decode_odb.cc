#include "eckit/testing/Test.h"

#include "odb_api/Reader.h"

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("The correct number of rows are decoded") {

    eckit::PathName filename("2000010106.odb");

    odb::Reader in(filename);
    odb::Reader::iterator it = in.begin();

    // Each table in the ODB will contain a maximum of 10000 rows!
    EXPECT(it->columns().rowsNumber() == 10000);

    size_t count = 0;
    for (; it != in.end(); ++it) {

        if (count < 3320000) {
            EXPECT(it->columns().rowsNumber() == 10000);
        } else {
            EXPECT(it->columns().rowsNumber() == 1753);
        }
        count++;
    }

    // All of the lines correctly decoded
    EXPECT(count == 3321753);
}


// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

