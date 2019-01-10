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

#include "odc/odc.h"

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

struct SetErrorHandling {
    SetErrorHandling(int type) { odc_error_handling(type); }
    ~SetErrorHandling() { odc_error_handling(ODC_THROW); }
};

// ------------------------------------------------------------------------------------------------------

CASE("Count lines in an existing ODB file") {

    SetErrorHandling e(ODC_ERRORS_REPORT);

    std::unique_ptr<odb_t> o(odc_open_for_read("../20000101067.odb"));
    EXPECT(odc_errno == 0);

    int ntables = odc_num_tables(o.get());
    EXPECT(odc_errno == 0);
    EXPECT(ntables == 333);

    size_t totalRows = 0;

    for (int i = 0; i < ntables; i++) {

        std::unique_ptr<odb_table_t> table(odc_get_table(o.get(), i));
        EXPECT(odc_errno == 0);
        totalRows += odc_table_num_rows(table.get());
        EXPECT(odc_errno == 0);
        EXPECT(odc_table_num_columns(table.get()) == 51);
        EXPECT(odc_errno == 0);
    }

    EXPECT(totalRows == 3321753);
}


CASE("Initialisation error") {

    SetErrorHandling e(ODC_ERRORS_REPORT);

    odc_initialise_api();

    if (odc_errno) {
        eckit::Log::info() << odc_error_string() << std::endl;
    }

    EXPECT(odc_errno == 0);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
