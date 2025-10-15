/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// This file contains examples of usage of public APIs.

#include <iostream>
#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileHandle.h"

#include "odc/api/Odb.h"

#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/Writer.h"

#include "TestCase.h"

using namespace eckit;

namespace {

TEST(example_select_data_read_results) {
    // Prepare input data
    const std::string data =
        R"(x:INTEGER,y:INTEGER,v:DOUBLE
        1,1,0.3
        1,1,0.2
        2,2,0.4
        2,2,0.1)";

    FileHandle out("example_select_data_read_results.odb");
    out.openForWrite(0);
    AutoClose close(out);
    odc::api::odbFromCSV(data, out);

    odc::Select select("select x,min(v),max(v);", "example_select_data_read_results.odb");

    for (odc::Select::iterator it(select.begin()), end(select.end()); it != end; ++it) {
        double r0 = (*it)[0], r1 = (*it)[1], r2 = (*it)[2];

        std::cout << r0 << ", " << r1 << ", " << r2 << std::endl;
    }
}


TEST(example_read_data) {
    // Prepare input data
    const std::string data =
        "x:INTEGER,y:INTEGER,v:DOUBLE\n"
        "1,1,0.3\n"
        "1,1,0.2\n"
        "2,2,0.4\n"
        "2,2,0.1\n";
    FileHandle out("example_read_data.odb");
    out.openForWrite(0);
    AutoClose close(out);
    odc::api::odbFromCSV(data, out);

    odc::Reader o("example_read_data.odb");
    for (odc::Reader::iterator it(o.begin()), end(o.end()); it != end; ++it) {
        double r0 = (*it)[0], r1 = (*it)[1], r2 = (*it)[2];

        std::cout << r0 << ", " << r1 << ", " << r2 << std::endl;
    }
}

TEST(example_write_data) {
    odc::core::MetaData metaData;
    metaData.addColumn("x", "INTEGER").addColumn("y", "INTEGER").addColumn("v", "DOUBLE");

    odc::Writer<> writer("example_write_data.odb");
    odc::Writer<>::iterator it(writer.begin());
    it->columns(metaData);
    it->writeHeader();

    for (size_t i(1); i <= 1000; ++i) {
        (*it)[0] = i;
        (*it)[1] = i * 2;
        (*it)[2] = i * 3;

        // Incrementing iterator moves coursor to the next row.
        ++it;
    }
}

}  // namespace
