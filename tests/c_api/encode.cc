/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>

#include "eckit/testing/Test.h"

#include "odc/api/odc.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

// TODO: Row-major
// TODO: Column major

CASE("Encode data in standard tabular form") {

    const int nrows = 15;
    const int ncols = 8;
    double data[nrows][ncols];

    for (int row = 0; row < nrows; ++row) {

//        row[row][0] = row;
//        row[row][1] = row;
//        row[row][2] = row;
//        row[row][3] = row;
    }

}

CASE("Encode data in standard tabular form with long strings") {

    const int nrows = 15;
    const int ncols = 8;
    double data[15][8];

//    for (int row = 0; row < )

}

// ------------------------------------------------------------------------------------------------------

CASE("Encode data in integral form") {

}

CASE("Encode data from custom striding") {

}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
