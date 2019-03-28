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

#include "eckit/utils/StringTools.h"
#include "eckit/testing/Test.h"

#include "odc/api/odc.h"

using namespace eckit::testing;

// Specialise custom deletion for odb_t

namespace std {
template <> struct default_delete<odb_t> {
    void operator() (odb_t* o) { odc_close(o); }
};

template <> struct default_delete<odb_table_t> {
    void operator() (odb_table_t* t) { odc_free_table(t); }
};

}

// Use a setter for the error handling to always revert to a known state.

struct SetErrorHandling {
    SetErrorHandling(int type) { odc_error_handling(type); }
    ~SetErrorHandling() { odc_error_handling(ODC_THROW); }
};

// ------------------------------------------------------------------------------------------------------

CASE("By default we throw on error") {

    // Don't explicitly set handler. Test default in first case.

    EXPECT_THROWS_AS(odc_open_for_read("../does-not-exist.odb"), eckit::CantOpenFile);
}

CASE("Correctly open an odb file") {

    SetErrorHandling e(ODC_THROW);

    std::unique_ptr<odb_t> o(odc_open_for_read("../2000010106.odb"));

    odb_table_t* t = odc_next_table(o.get(), false);
    EXPECT(t != 0);
    EXPECT(odc_errno == 0);
    odc_free_table(t);
}

CASE("We can report errors through the API") {

    SetErrorHandling e(ODC_ERRORS_REPORT);

    std::unique_ptr<odb_t> o(odc_open_for_read("../does-not-exist.odb"));

    std::string expected = "Cannot open ../does-not-exist.odb"; // test start since strerror_r() message isn't portable

    EXPECT(!o);
    EXPECT(odc_errno != 0);
    EXPECT(odc_error_string() != 0);

    EXPECT(eckit::StringTools::startsWith(odc_error_string(), expected));

    o.reset(odc_open_for_read("../does-not-exist.odb"));

    EXPECT(!o);
    EXPECT(odc_errno != 0);
    EXPECT(odc_error_string() != 0);
    EXPECT(eckit::StringTools::startsWith(odc_error_string(), expected));
}

CASE("Don't continue unless error has been reset") {

    SetErrorHandling e(ODC_ERRORS_CHECKED);

    std::unique_ptr<odb_t> o(odc_open_for_read("../does-not-exist.odb"));

    std::string expected = "Cannot open ../does-not-exist.odb"; // test start since strerror_r() message isn't portable

    EXPECT(!o);
    EXPECT(odc_errno != 0);
    EXPECT(odc_error_string() != 0);
    EXPECT(eckit::StringTools::startsWith(odc_error_string(), expected));

    // Reset error condition (in principle handle it) then try again
    odc_reset_error();

    o.reset(odc_open_for_read("../does-not-exist.odb"));

    EXPECT(!o);
    EXPECT(odc_errno != 0);
    EXPECT(odc_error_string() != 0);
    EXPECT(eckit::StringTools::startsWith(odc_error_string(), expected));

    // We haven't handled the previous error, so the API will barf

    EXPECT_THROWS_AS(odc_open_for_read("../does-not-exist.odb"), eckit::SeriousBug);

    // Reset error before cleanup
    odc_reset_error();
}


// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
