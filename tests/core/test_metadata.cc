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

#include "odc/Reader.h"
#include "odc/Writer.h"
#include "odc/core/MetaData.h"

using namespace eckit::testing;
using eckit::Log;

// ------------------------------------------------------------------------------------------------------

CASE("Copy metadata preserves data sizes") {

    odc::core::MetaData md;
    md.addColumn("col1", "STRING");
    md.back()->dataSizeDoubles(2);
    md.addColumn("col2", "STRING");

    EXPECT(md[0]->dataSizeDoubles() == 2);

    odc::core::MetaData md2;
    md2 = md;

    EXPECT(md.size() == 2);
    EXPECT(md[0]->dataSizeDoubles() == 2);
    EXPECT(md[1]->dataSizeDoubles() == 1);
}


CASE("Copy metadata in Writer") {

    odc::core::MetaData md;
    md.addColumn("col1", "STRING");
    md.back()->dataSizeDoubles(2);
    md.addColumn("col2", "STRING");

    EXPECT(md[0]->dataSizeDoubles() == 2);

    odc::Writer<> w("example.odb");
    odc::Writer<>::iterator it = w.begin();

    it->columns(md);

    eckit::Log::info() << "Found: " << it->columns().at(0)->dataSizeDoubles() << std::endl;
    EXPECT(it->columns().at(0)->dataSizeDoubles() == 2);
    EXPECT(it->columns().at(1)->dataSizeDoubles() == 1);
}


CASE("Missing/ambiguous column names") {
    odc::Reader reader("../2000010106-reduced.odb");
    auto it = reader.begin();
    EXPECT(it != reader.end());
    const odc::core::MetaData& md = it->columns();
    EXPECT(md.columnIndex("event1@hdr") == 11);
    EXPECT(md.columnIndex("event1@body") == 35);
    EXPECT_THROWS_AS(md.columnIndex("event1"), odc::core::AmbiguousColumnException);
    EXPECT_THROWS_AS(md.columnIndex("bogus"), odc::core::ColumnNotFoundException);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
