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

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"

#include "odc/core/TablesReader.h"

using namespace eckit::testing;
using eckit::Log;


// ------------------------------------------------------------------------------------------------------

eckit::Resource<eckit::PathName> testDataPath("$TEST_DATA_DIRECTORY", "..");

CASE("Test access Table iterator") {

    eckit::PathName filename = testDataPath / "2000010106.odb";

    std::unique_ptr<eckit::DataHandle> dh(filename.fileHandle());
    dh->openForRead();
    eckit::AutoClose close(*dh);

    odc::core::TablesReader reader(*dh);
    auto it = reader.begin();
    auto end = reader.end();

    size_t numRows = 0;
    size_t tableCount = 0;
    eckit::Offset lastOffset = 0;

    EXPECT(dh->estimate() == eckit::Length(155557962));

    while (it != end) {
        tableCount++;

        EXPECT(it->rowCount() == (tableCount == 333? 1753 : 10000));
        EXPECT(it->nextPosition() > lastOffset);
        EXPECT(it->nextPosition() <= dh->estimate());
        EXPECT(dh->estimate() == eckit::Length(155557962));
        lastOffset = it->nextPosition();

        numRows += it->rowCount();
        ++it;
    }

    EXPECT(dh->estimate() == eckit::Length(155557962));
    EXPECT(lastOffset == eckit::Offset(155557962));
    EXPECT(numRows == 3321753);

    dh->close();
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

