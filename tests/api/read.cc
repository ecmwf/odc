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

// ------------------------------------------------------------------------------------------------------

CASE("Count lines in an existing ODB file") {

    std::unique_ptr<odb_t> o(odc_open_for_read("../2000010106.odb"));

    int ntables = odc_num_tables(o.get());

    size_t totalRows = 0;

    for (int i = 0; i < ntables; i++) {

        std::unique_ptr<odb_table_t> table(odc_get_table(o.get(), i));
        totalRows += odc_table_num_rows(table.get());
        EXPECT(odc_table_num_columns(table.get()) == 51);
    }

    EXPECT(totalRows == 3321753);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
