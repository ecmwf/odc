/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "odc/Writer.h"
#include "odc/Select.h"

#include "TemporaryFiles.h"

#include <iomanip>

using namespace eckit::testing;
using eckit::types::is_approximately_equal;

// ------------------------------------------------------------------------------------------------------

CASE("EQ_BOXLAT and EQ_BOXLON") {

    TemporaryFile f;

    // Write some data with a latitude and longitude.

    {
        odc::Writer<> oda(f.path());
        odc::Writer<>::iterator row = oda.begin();

        row->setNumberOfColumns(2);

        row->setColumn(0, "lat", odc::api::REAL);
        row->setColumn(1, "lon", odc::api::REAL);

        row->writeHeader();

        // Include some extreme values

        (*row)[0] = 45.0;
        (*row)[1] = 0.0;
        ++row;

        (*row)[0] = 90.0;
        (*row)[1] = 180.0;
        ++row;

        (*row)[0] = -90.0;
        (*row)[1] = 360.0;
        ++row;

        (*row)[0] = -45.0;
        (*row)[1] = -90.0;
        ++row;
    }

    // And test that the SQL functions get the right data out!!!

    const std::string sql = std::string("select eq_boxlat(lat,lon,10.5), eq_boxlon(lat,lon,10.5) ") +
                                        "from \"" + f.path() + "\";";

    const double eps = 7.e-6;

    {
        odc::Select oda(sql);
        odc::Select::iterator it = oda.begin();

        EXPECT(is_approximately_equal((*it)[0], 47.134066345052e0, eps));
        EXPECT((*it)[1] == 0);

        ++it;

        EXPECT(is_approximately_equal((*it)[0], 87.03598391, eps));
        EXPECT((*it)[1] == 0);

        ++it;

        EXPECT(is_approximately_equal((*it)[0], -87.03598391, eps));
        EXPECT((*it)[1] == 0);

        ++it;

        EXPECT(is_approximately_equal((*it)[0], -47.13406635, eps));
        EXPECT(is_approximately_equal((*it)[1], -93.91304348, eps));

        ++it;

        EXPECT(it == oda.end());
    }
}

CASE("RGG_BOXLAT and RGG_BOXLON") {

    TemporaryFile f;

    // Write some data with a latitude and longitude.

    {
        odc::Writer<> oda(f.path());
        odc::Writer<>::iterator row = oda.begin();

        row->setNumberOfColumns(2);

        row->setColumn(0, "lat", odc::api::REAL);
        row->setColumn(1, "lon", odc::api::REAL);

        row->writeHeader();

        // Include some extreme values

        (*row)[0] = 45.0;
        (*row)[1] = 0.0;
        ++row;

        (*row)[0] = 90.0;
        (*row)[1] = 180.0;
        ++row;

        (*row)[0] = -90.0;
        (*row)[1] = 360.0;
        ++row;

        (*row)[0] = -45.0;
        (*row)[1] = -90.0;
        ++row;
    }

    // And test that the SQL functions get the right data out!!!

    const std::string sql = std::string("select rgg_boxlat(lat,lon,31), rgg_boxlon(lat,lon,31) ") +
                                        "from \"" + f.path() + "\";";

    const double eps = 7.e-6;

    {
        odc::Select oda(sql);
        odc::Select::iterator it = oda.begin();

        EXPECT(is_approximately_equal((*it)[0], 47.06964206, eps));
        EXPECT((*it)[1] == 0);

        ++it;

        EXPECT(is_approximately_equal((*it)[0], 85.76058712, eps));
        EXPECT((*it)[1] == 180);

        ++it;

        EXPECT(is_approximately_equal((*it)[0], -85.76058712, eps));
        EXPECT((*it)[1] == 0);

        ++it;

        EXPECT(is_approximately_equal((*it)[0], -47.06964206, eps));
        EXPECT((*it)[1] == -90);

        ++it;

        EXPECT(it == oda.end());
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
