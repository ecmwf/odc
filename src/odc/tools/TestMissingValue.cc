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

#include "eckit/sql/SQLTypedefs.h"

#include "odc/Comparator.h"
#include "odc/Select.h"
#include "odc/Reader.h"

#include "odc/Tracer.h"
#include "odc/Writer.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;
static void setUp()
{
	Tracer t(Log::debug(), "setUp");

	odc::Writer<> f("TestMissingValue.odb");
	odc::Writer<>::iterator it = f.begin();

    it->setNumberOfColumns(2);

	it->setColumn(0, "lat@hdr", odc::REAL);
	it->missingValue(0, 1);

    eckit::sql::BitfieldDef bfDef;
	bfDef.first.push_back("x");
	bfDef.second.push_back(1);
	bfDef.first.push_back("y");
	bfDef.second.push_back(2);

	it->setBitfieldColumn(1, "bf", odc::BITFIELD, bfDef);

	it->writeHeader();

	for (size_t i = 0; i <= 2; i++)
	{
		(*it)[0] = i;
		(*it)[1] = i;
		++it;
	}
}

static void selectIntoSecondFile()
{
    Tracer t(Log::debug(), "selectIntoSecondFile");

    const string fileName = "TestMissingValue.odb";
    string sql = "select lat,bf into \"TestMissingValue.odb\"";
    sql += " from \"" + fileName + "\" ;";

    odc::Select f(sql); //, fileName);
    odc::Select::iterator it = f.begin();

    ++it; // this is needed to push the second row to the INTO file
    ++it; // this is needed to push the third row to the INTO file
}


static void test()
{
	selectIntoSecondFile();

	odc::Comparator().compare("TestMissingValue.odb", "TestMissingValue.odb");

	{
		odc::Reader f("TestMissingValue.odb");
		odc::Reader::iterator fbegin(f.begin());
		odc::Reader::iterator fend(f.end());

		odc::Select s("select * from \"TestMissingValue.odb\";");
		odc::Select::iterator sbegin(s.begin());
		odc::Select::iterator send(s.end());

		odc::Comparator().compare(fbegin, fend, sbegin, send, "TestMissingValue.odb", "SELECT TestMissingValue.odb");
	}

	{
		odc::Reader f("TestMissingValue.odb");
		odc::Reader::iterator it = f.begin();
		odc::Reader::iterator end = f.end();

        Column& column = *it->columns()[0];
		codec::Codec& codec = column.coder();

		Log::info() << "test: codec: " << codec << std::endl;	

		ASSERT(codec.hasMissing());
		ASSERT(codec.missingValue() == 1);


		for (; it != end; ++it)
		{
			ASSERT( (*it).missingValue(0) == 1 );

			if ( (*it)[0] == 1 )
				ASSERT( (*it).isMissing(0) );
			else
				ASSERT( ! (*it).isMissing(0) );
		}
	}

	{
		// Check the isMissing and missingValue API of SelectIterator
		odc::Select s("select * from \"TestMissingValue.odb\";"); //, fileName);
		odc::Select::iterator i = s.begin();
		odc::Select::iterator e = s.end();
		for (; i != e; ++i)
		{
			ASSERT( (*i).missingValue(0) == 1 );
			ASSERT( (*i).missingValue(1) == 0 );

			if ( (*i)[0] == 1 )
				ASSERT( (*i).isMissing(0) );
			else
				ASSERT( ! (*i).isMissing(0) );

			// For Bitfields missing value by default equals 0
			if ( (*i)[1] == 0 )
				ASSERT( (*i).isMissing(1) );
			else
				ASSERT( ! (*i).isMissing(1) );
		}
	}

}


static void tearDown() { }



SIMPLE_TEST(MissingValue)
