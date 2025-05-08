/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/log/Log.h"

#include "odc/LibOdc.h"
#include "odc/Select.h"
#include "odc/Writer.h"
#include "odc/utility/Tracer.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;
using namespace odc::utility;

static void setUp() {
    Tracer t(Log::debug<LibOdc>(), "setUp");
    {
        odc::Writer<> f("TestSelectTwoFiles1.odb");
        odc::Writer<>::iterator it = f.begin();
        it->setNumberOfColumns(1);
        it->setColumn(0, "a", odc::api::REAL);
        it->writeHeader();
        (*it)[0] = 1;
        ++it;
    }
    {
        odc::Writer<> f("TestSelectTwoFiles2.odb");
        odc::Writer<>::iterator it = f.begin();
        it->setNumberOfColumns(1);
        it->setColumn(0, "b", odc::api::REAL);
        it->writeHeader();
        (*it)[0] = 2;
        ++it;
    }
}

static void test() {
    Tracer t(Log::debug<LibOdc>(), "test");

    odc::Select s("select * from \"TestSelectTwoFiles1.odb\", \"TestSelectTwoFiles2.odb\";");
    odc::Select::iterator it  = s.begin();
    odc::Select::iterator end = s.end();

    ASSERT(it->columns().size() == 2);

    unsigned long i = 0;
    for (; it != end; ++it) {
        LOG_DEBUG_LIB(LibOdc) << "test:    " << (*it)[0] << "    " << (*it)[0] << std::endl;

        ASSERT(((*it)[0] == 1) && ((*it)[1] == 2));
        ++i;
    }

    ASSERT(i == 1);
}

static void tearDown() {}

SIMPLE_TEST(SelectTwoFiles)
