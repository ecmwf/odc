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

#include "eckit/exception/Exceptions.h"
#include "odblib/MetaData.h"
#include "odblib/Reader.h"

#include "odblib/Writer.h"
#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

const size_t numberOfColumns = 1;


static void setUp()
{

    odb::Writer<> oda("UnitTest.odb");
    odb::Writer<>::iterator row = oda.begin();

    row->setNumberOfColumns(numberOfColumns);

    row->setColumn(0, "x", odb::REAL);
    //row->setColumn(1, "y", odb::REAL);
    //row->setColumn(2, "z", odb::REAL);

    row->writeHeader();

    //for (size_t i = 1; i <= 2; i++)
    //{
    size_t i = 1;
    (*row)[0] = i; // col 0
    //(*row)[1] = i; // col 1
    //(*row)[2] = i; // col 2
    ++row;
    //}
    row->close();

}

static void test()
{

    odb::Reader reader("UnitTest.odb");
    odb::Reader::iterator it = reader.begin();
    const odb::MetaData& md = it->columns();

    Log::info() << "testSelectIterator: md = " << md << std::endl;
    ASSERT(md.size() == numberOfColumns);

    string codecName = md[0]->coder().name();

    Log::info() << "testSelectIterator: codecName = " << codecName << std::endl;

    ASSERT("All columns should be constant" && codecName == "constant");
    return;
}


static void tearDown(){}

RUN_SIMPLE_TEST
