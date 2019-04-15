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
#include "eckit/io/MemoryHandle.h"

#include "odc/api/Odb.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

CASE("We can import data") {

    const char* SOURCE_DATA =
        R"(col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER,col5:BITFIELD[a:1;b:2;c:5]
        1,1.23,4.56,7,999
        123,0.0,0.0,321,888
        321,0.0,0.0,123,777
        0,3.25,0.0,0,666
        0,0.0,3.25,0,555)";

    eckit::MemoryHandle dh_out;
    size_t importedSize;

    {
        dh_out.openForWrite(0);
        eckit::AutoClose close(dh_out);
        ::odc::api::odbFromCSV(SOURCE_DATA, dh_out);
        importedSize = dh_out.position();
    }

    eckit::Log::info() << "Imported length: " << importedSize << std::endl;

    eckit::MemoryHandle readAgain(dh_out.data(), importedSize);
    readAgain.openForRead();
    odc::api::Odb o(readAgain);

    while (auto table = o.next(false)) {
        eckit::Log::info() << "Table: " << table.get().rowCount() << std::endl;
    }



}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
