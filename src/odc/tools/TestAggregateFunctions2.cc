/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAggregateFunctions2.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#include "eckit/io/StdFileHandle.h"
#include "eckit/io/FileHandle.h"

#include "odc/api/Odb.h"
#include "odc/Select.h"
#include "odc/Writer.h"

#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

static void test()
{
	string sql = "select count(*) from \"TestAggregateFunctions2.odb\";";

	Log::info() << "Executing: '" << sql << "'" << std::endl;

	odc::Select oda(sql);
	odc::Select::iterator it = oda.begin();

	ASSERT(it->columns().size() == 1);
	ASSERT((*it)[0] == 10); 

	odc::Select sel(sql);
	odc::Select::iterator it2 = sel.begin();
	odc::Select::iterator end2 = sel.end();

	FILE *fout = fopen("TestAggregateFunctions2.odb", "w");
	StdFileHandle fhout(fout);
	odc::Writer<> writer(fhout);
	odc::Writer<>::iterator outit = writer.begin();

	//outit->pass1(it2, end2);
	size_t i = 0;
	for (; it2 != end2; ++it2)
	{
		++i;
		ASSERT( (*it2)[0] == 10);
	}
	ASSERT( i == 1);
}

static void setUp()
{
	stringstream s;
	s << "a:REAL" << std::endl;
	for (size_t i = 1; i <= 10; ++i)
		s << i << std::endl;
    FileHandle dh("TestAggregateFunctions2.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(s, dh);
}


static void tearDown(){}

SIMPLE_TEST(AggregateFunctions2)
