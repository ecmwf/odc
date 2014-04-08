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

#include "eckit/log/Timer.h"
#include "odblib/ODBAPISettings.h"

#include "odblib/Writer.h"
#include "eckit/testing/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;



static void setUp() { }


static void createFile(size_t numberOfColumns, long long numberOfRows, size_t setvbufferSize)
{

	ODBAPISettings::instance().setvbufferSize(setvbufferSize);

	odb::Writer<> oda("UnitTest.odb");
	odb::Writer<>::iterator row = oda.begin();

    row->setNumberOfColumns(numberOfColumns);

	for (size_t i = 0; i < numberOfColumns; ++i)
	{
		stringstream name;
		name << "Column" << i;
		row->setColumn(i, name.str().c_str(), odb::REAL);
	}
	row->writeHeader();

	for (long long i = 1; i <= numberOfRows; ++i, ++row)
		for (size_t c = 0; c < numberOfColumns; ++c)
			(*row)[c] = c;
}

static void tearDown()
{
	int catStatus = system("ls -l UnitTest.odb");
	ASSERT(WEXITSTATUS(catStatus) == 0);
}


static void test()
{
    size_t cols = 400;
    long long rows = 1000;
    size_t buffSize = 8 * 1024 * 1024;

    for (size_t i = 0; i < 10; ++i)
    {
        stringstream s;
        s << "setUp(): createFile(" << cols << ", " << rows << ", " << buffSize << ")" << std::endl;
        Timer t(s.str());
        createFile(cols, rows, buffSize);
    }
}



RUN_SIMPLE_TEST
