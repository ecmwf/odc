/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.cc
///
/// @author ECMWF, July 2010

const double EPS = 7e-6;

#include "eckit/log/Timer.h"
#include "odc/Select.h"

#include "TestCase.h"
#include "odc/Writer.h"

#include <cmath>

using namespace std;
using namespace eckit;
using namespace odc;


static void setUp() {
    Timer t("Test various functions to convert angles (radians to degrees, etc.)");
    odc::Writer<> oda("test_angleconv.odb");

    odc::Writer<>::iterator row = oda.begin();
    row->setNumberOfColumns(2);

    row->setColumn(0, "radian_col", odc::api::REAL);
    row->setColumn(1, "degrees_col", odc::api::REAL);

    row->writeHeader();

    (*row)[0] = M_PI;
    (*row)[1] = 180.0e0;
    ++row;

    (*row)[0] = 0.0e0;
    (*row)[1] = 0.0e0;
    ++row;

    (*row)[0] = M_PI / 4.0e0;
    (*row)[1] = 45.0e0;
    ++row;
}

static void tearDown() {
    PathName("test_angleconv.odb").unlink();
}

static void test() {
    const string sql =
        "select degrees(radian_col),radians(degrees_col), rad2deg(radian_col), deg2rad(degrees_col), "
        "radians(degrees(radian_col)), degrees(radians(degrees_col)) from \"test_angleconv.odb\";";

    Log::info() << "Executing: '" << sql << "'" << std::endl;

    odc::Select oda(sql);
    odc::Select::iterator it = oda.begin();

    // because stored as single real precision; we loose some accuracy
    ASSERT(fabs((*it)[0] - 180) < EPS);    //
    ASSERT(fabs((*it)[1] - M_PI) < EPS);   //
    ASSERT(fabs((*it)[2] - 180.0) < EPS);  //
    ASSERT(fabs((*it)[3] - M_PI) < EPS);   //

    ASSERT(fabs((*it)[4] - M_PI) < EPS);     //
    ASSERT(fabs((*it)[5] - 180.0e0) < EPS);  //

    ++it;
    ASSERT(fabs((*it)[0]) < EPS);  //
    ASSERT(fabs((*it)[1]) < EPS);  //
    ASSERT(fabs((*it)[2]) < EPS);  //
    ASSERT(fabs((*it)[3]) < EPS);  //

    ASSERT(fabs((*it)[4]) < EPS);  //
    ASSERT(fabs((*it)[5]) < EPS);  //

    ++it;
    ASSERT(fabs((*it)[0] - 45) < EPS);          //
    ASSERT(fabs((*it)[1] - M_PI / 4.0) < EPS);  //
    ASSERT(fabs((*it)[2] - 45.0) < EPS);        //
    ASSERT(fabs((*it)[3] - M_PI / 4.0) < EPS);  //

    ASSERT(fabs((*it)[4] - M_PI / 4.0) < EPS);  //
    ASSERT(fabs((*it)[5] - 45.0e0) < EPS);      //
}


SIMPLE_TEST(FunctionsForAngleConversion)
