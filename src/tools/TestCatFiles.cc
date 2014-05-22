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

#include "odb_api/odblib/Writer.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void setUp()
{
    {
        odb::Writer<> oda("file1.odb");
        odb::Writer<>::iterator row = oda.begin();

        row->setNumberOfColumns(3);

        row->setColumn(0, "x", odb::REAL);
        row->setColumn(1, "y", odb::REAL);
        row->setColumn(2, "z", odb::REAL);

        row->writeHeader();

        for (size_t i = 1; i <= 2; i++)
        {
            (*row)[0] = i; // col 0
            (*row)[1] = i; // col 1
            (*row)[2] = i; // col 2
            ++row;
        }

        odb::Writer<> oda2("file2.odb");
        odb::Writer<>::iterator row2 = oda2.begin();
        row2->setNumberOfColumns(3);
        row2->setColumn(0, "x", odb::REAL);
        row2->setColumn(1, "y", odb::REAL);
        row2->setColumn(2, "v", odb::REAL);

        row2->writeHeader();

        for (size_t i = 1; i <= 2; i++)
        {
            (*row2)[0] = i * 10; // col 0
            (*row2)[1] = i * 100; // col 1
            (*row2)[2] = i * 1000; // col 2
            ++row2;
        }

        odb::Writer<> oda3("file3.odb");
        odb::Writer<>::iterator row3 = oda3.begin();
        row3->setNumberOfColumns(4);

        row3->setColumn(0, "x", odb::REAL);
        row3->setColumn(1, "v", odb::REAL);
        row3->setColumn(2, "y", odb::REAL);
        row3->setColumn(3, "z", odb::REAL);
        row3->writeHeader();

        for (size_t i = 1; i <= 2; i++)
        {
            (*row3)[0] = i * 10; // col 0
            (*row3)[1] = i * 1000; // col 1
            (*row3)[2] = i * 100; // col 2
            (*row3)[3] = 13;     // col 3
            ++row3;
        }
        // dtors fire here
    }

    int catStatus = system("cat file1.odb file2.odb file3.odb >concatenated.odb");
    ASSERT(WEXITSTATUS(catStatus) == 0);

}


static void test()
{
    const string sql = "select X,Y from \"concatenated.odb\";";
    const string fileName = "concatenated.odb";
    odb::Select oda(sql, fileName);
    Log::info() << "Iterating " << sql << std::endl;

    //for (Reader::iterator it = oda.begin();
    //	it != oda.end();
    //	++it)
    //{}

    odb::Select::iterator it = oda.begin();
    int j = 1;

    Log::info(Here()) << "j = " << j << std::endl;
    const double * data = it->data();
    ASSERT(data);
    Log::info(Here()) << "data[0] = " << data[0] << std::endl;
    Log::info(Here()) << "data[1] = " << data[1] << std::endl;
    ASSERT(data[0] == j);
    ASSERT(data[1] == j);
    ++j;

    ++it;
    Log::info(Here()) << "j = " << j << std::endl;
    data = it->data();
    ASSERT(data);
    Log::info(Here()) << "data[0] = " << data[0] << std::endl;
    Log::info(Here()) << "data[1] = " << data[1] << std::endl;
    ASSERT(data[0] == j);
    ASSERT(data[1] == j);
    ++j;

    j = 1; // data from file2.oda

    ++it;
    Log::info(Here()) << "j = " << j << std::endl;
    data = it->data();
    ASSERT(data);
    Log::info(Here()) << "data[0] = " << data[0] << std::endl;
    Log::info(Here()) << "data[1] = " << data[1] << std::endl;
    ASSERT(data[0] == j * 10);
    ASSERT(data[1] == j * 100);
    ++j;

    ++it;
    Log::info(Here()) << "j = " << j << std::endl;
    data = it->data();
    ASSERT(data);
    Log::info(Here()) << "data[0] = " << data[0] << std::endl;
    Log::info(Here()) << "data[1] = " << data[1] << std::endl;
    ASSERT(data[0] == j * 10);
    ASSERT(data[1] == j * 100);


    j = 1; // data from file3.oda

    ++it;
    Log::info(Here()) << "j = " << j << std::endl;
    data = it->data();
    ASSERT(data);
    Log::info(Here()) << "data[0] = " << data[0] << std::endl;
    Log::info(Here()) << "data[1] = " << data[1] << std::endl;
    ASSERT(data[0] == j * 10);
    ASSERT(data[1] == j * 100);
    ++j;

    ++it;
    Log::info(Here()) << "j = " << j << std::endl;
    data = it->data();
    ASSERT(data);
    Log::info(Here()) << "data[0] = " << data[0] << std::endl;
    Log::info(Here()) << "data[1] = " << data[1] << std::endl;
    ASSERT(data[0] == j * 10);
    ASSERT(data[1] == j * 100);
}



static void tearDown(){}

SIMPLE_TEST(CatFiles)
