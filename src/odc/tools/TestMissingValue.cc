/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestMissingValue.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/log/Log.h"
#include "eckit/sql/SQLTypedefs.h"

#include "odc/Comparator.h"
#include "odc/LibOdc.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/Writer.h"
#include "odc/utility/Tracer.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;
using namespace odc::utility;
using namespace odc::core;

static void setUp() {
    Tracer t(Log::debug<LibOdc>(), "setUp");

    odc::Writer<> f("TestMissingValue.odb");
    odc::Writer<>::iterator it = f.begin();

    it->setNumberOfColumns(2);

    it->setColumn(0, "lat@hdr", odc::api::REAL);
    it->missingValue(0, 1);

    eckit::sql::BitfieldDef bfDef;
    bfDef.first.push_back("x");
    bfDef.second.push_back(1);
    bfDef.first.push_back("y");
    bfDef.second.push_back(2);

    it->setBitfieldColumn(1, "bf", odc::api::BITFIELD, bfDef);
    it->missingValue(1, 0);

    it->writeHeader();

    for (size_t i = 0; i <= 2; i++) {
        (*it)[0] = i;
        (*it)[1] = i;
        ++it;
    }
}

static void selectIntoSecondFile() {
    Tracer t(Log::debug<LibOdc>(), "selectIntoSecondFile");

    const string fileName = "TestMissingValue.odb";
    string sql            = "select lat,bf into \"TestMissingValue2.odb\"";
    sql += " from \"" + fileName + "\" ;";

    odc::Select f(sql);  //, fileName);
    odc::Select::iterator it = f.begin();

    ++it;  // this is needed to push the second row to the INTO file
    ++it;  // this is needed to push the third row to the INTO file
}


static void test() {
    selectIntoSecondFile();

    odc::Comparator().compare("TestMissingValue.odb", "TestMissingValue2.odb");

    {
        odc::Reader f("TestMissingValue.odb");
        odc::Reader::iterator fbegin(f.begin());
        odc::Reader::iterator fend(f.end());

        odc::Select s("select * from \"TestMissingValue2.odb\";");
        odc::Select::iterator sbegin(s.begin());
        odc::Select::iterator send(s.end());

        odc::Comparator().compare(fbegin, fend, sbegin, send, "TestMissingValue.odb", "SELECT TestMissingValue2.odb");
    }

    {
        odc::Reader f("TestMissingValue2.odb");
        odc::Reader::iterator it  = f.begin();
        odc::Reader::iterator end = f.end();

        Column& column = *it->columns()[0];
        Codec& codec   = column.coder();

        ASSERT(codec.hasMissing());

        // We do not preserve missing values across these calls. Missing value is as
        // configured in the encoder, not the data source
        //		ASSERT(codec.missingValue() == 1);
        ASSERT(codec.missingValue() == MDI::realMDI());

        for (int row = 0; it != end; ++it, ++row) {
            if (row == 1) {
                ASSERT((*it).isMissing(0));
            }
            else {
                ASSERT(!(*it).isMissing(0));
            }
        }
    }

    {
        // Check that we correctly identify missing items in the select api
        // n.b. We _don't' expose the original missing value...
        odc::Select s("select * from \"TestMissingValue2.odb\";");  //, fileName);
        odc::Select::iterator i = s.begin();
        odc::Select::iterator e = s.end();
        for (int row = 0; i != e; ++i, ++row) {
            ASSERT((*i).missingValue(0) == MDI::realMDI());
            ASSERT((*i).missingValue(1) == MDI::bitfieldMDI());

            if (row == 0) {
                ASSERT(!(*i).isMissing(0));
                ASSERT((*i).isMissing(1));
            }
            else if (row == 1) {
                ASSERT((*i).isMissing(0));
                ASSERT(!(*i).isMissing(1));
            }
            else {
                ASSERT(!(*i).isMissing(0));
                ASSERT(!(*i).isMissing(1));
            }
        }
    }
}


static void tearDown() {}


SIMPLE_TEST(MissingValue)
