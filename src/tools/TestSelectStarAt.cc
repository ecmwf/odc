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

#include "odb_api/odblib/MetaData.h"
#include "odb_api/odblib/Select.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;

/// UnitTest expansion of '*@hdr' into a list of columns of the hdr ODB table.
///
static void test()
{
    // TODO: make sure a 'select ... into ... from ...', e.g.:
    //
    //	const string SELECT = "select *@hdr into \"out.odb\" from \"2000010106.odb\";";
    //  is not returning a result set (iterator). Or perhaps it is returning an empty result set.

    const string SELECT = "select *@hdr from \"2000010106.odb\";";

    odb::Select oda(SELECT);

    Log::info() << "Executing: '" << SELECT << "'" << std::endl;
    odb::Select::iterator it = oda.begin();

    Log::info() << "it->columns().size() => " << it->columns().size() << std::endl;
    ASSERT(it->columns().size() == 27);

#if 0
    unsigned long long i = 0;
    for ( ; it != oda.end(); ++it)
        ++i;

    Log::info() << "i == " << i << std::endl;
    ASSERT(i == 3321753);
#endif
}



static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(SelectStarAt)
