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
#include "odc/api/Odb.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

CASE("Count lines in an existing ODB file") {

    odc::api::Odb o("../2000010106.odb");

    size_t ntables = 0;
    size_t totalRows = 0;

    while (const auto& table = o.next(false)) {
        totalRows += table.get().numRows();
        EXPECT(table.get().numColumns() == 51);
        ++ntables;
    }

    EXPECT(ntables == 333);
    EXPECT(totalRows == 3321753);
}

// ------------------------------------------------------------------------------------------------------

//CASE("Decode an entire ODB file") {
//
//    odc::api::Odb o("../2000010106.odb");
//
//    size_t ntables = 0;
//
//    while (const auto& table = o.next()) {
//
//        std::unique_ptr<const odb_decoded_t> decoded(odc_table_decode_all(table.get()));
//        EXPECT(decoded->nrows == odc_table_num_rows(table.get()));
//        EXPECT(decoded->ncolumns == 51);
//
//        ++ntables;
//    }
//}

// ------------------------------------------------------------------------------------------------------

CASE("Decode an entire ODB file preallocated data structures") {
//
//    std::unique_ptr<odb_t> o(odc_open_for_read("../2000010106.odb"));
//
//    int ntables = odc_num_tables(o.get());
//    EXPECT(ntables == 333);
//
//    odb_decoded_t decoded;
//    odb_strided_data_t strided_data[51];
//
//    for (int i = 0; i < ntables; i++) {
//
//        std::unique_ptr<odb_table_t> table(odc_get_table(o.get(), i));
//
//        ASSERT(odc_table_num_columns(table.get()) == 51);
//
//        decoded.ncolumns = 51;
//        decoded.nrows = 10000;
//        decoded.columnData = strided_data;
//
//        ///   odc_table_decode(table.get(), &decoded);
//
//        ///EXPECT(decoded.nrows == odc_table_num_rows(table.get()));
//        ///EXPECT(decoded.ncolumns == 51);
//
//        ///eckit::Log::info() << "Decoded: ncolumns = " << decoded.ncolumns << std::endl;
//        ///eckit::Log::info() << "Decoded: nrows = " << decoded.nrows << std::endl;
//        ///eckit::Log::info() << "Decoded: data = " << decoded.columnData << std::endl;
//    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
