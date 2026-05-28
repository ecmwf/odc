/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <limits>

#include "eckit/testing/Test.h"

#include "odc/Comparator.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

CASE("Comparator treats two NaNs as equal") {
    const double NaN = std::numeric_limits<double>::quiet_NaN();

    EXPECT(odc::Comparator::same(NaN, NaN));
    EXPECT(!odc::Comparator::same(NaN, 1.0));
    EXPECT(!odc::Comparator::same(1.0, NaN));
}

CASE("Comparator treats identical values as equal") {
    EXPECT(odc::Comparator::same(0.0, 0.0));
    EXPECT(odc::Comparator::same(1.0, 1.0));
    EXPECT(odc::Comparator::same(-1.0, -1.0));
}

CASE("Comparator treats different values as unequal") {
    EXPECT(!odc::Comparator::same(0.0, 1.0));
    EXPECT(!odc::Comparator::same(1.0, 2.0));
    EXPECT(!odc::Comparator::same(1.0, -1.0));
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
