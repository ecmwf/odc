/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "odb_api/Writer.h"
#include "odb_api/Select.h"

#include "TemporaryFiles.h"

using namespace eckit::testing;
using eckit::types::is_approximately_equal;

// ------------------------------------------------------------------------------------------------------

CASE("Various distance measuring functions return sensible things") {

    TemporaryFile f;

    // Write some data with a latitude and longitude.

    {
        odb::Writer<> oda(f.path());
        odb::Writer<>::iterator row = oda.begin();

        row->setNumberOfColumns(2);

        row->setColumn(0, "lat", odb::REAL);
        row->setColumn(1, "lon", odb::REAL);

        row->writeHeader();

        (*row)[0] = 45.0;
        (*row)[1] = 0.0;

        ++row;
    }

    // And test that the SQL functions get the right data out!!!

    const std::string sql = std::string("select rad(45.0,0.0,1.0,lat,lon), ") +
                                        "rad(10.0,0.0,0.0,lat,lon), " +
                                        "distance(46.0,0.0,lat,lon), " +
                                        "km(46.0,0.0,lat,lon), " +
                                        "dist(100.,46.0,1.0,lat,lon), " +
                                        "dist(40.0,5.0,1000.0,lat,lon) " +
                                        "from \"" + f.path() + "\";";

    const double eps = 7.e-6;

    {
        odb::Select oda(sql);
        odb::Select::iterator it = oda.begin();


        EXPECT((*it)[0] == 1);                                  // Inside relevant great-circle
        EXPECT((*it)[1] == 0);
        ASSERT(is_approximately_equal((*it)[2], 111120., eps)); // Surface distance to specified point
        ASSERT(is_approximately_equal((*it)[3], 111.12, eps));  // In m
        ASSERT((*it)[4] == 0);                                  // Within specified distance of given point
        ASSERT((*it)[5] == 1);
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
