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

#include "odblib/Comparator.h"
#include "odblib/MetaData.h"
#include "odblib/Select.h"
#include "odblib/Reader.h"

#include "odblib/Writer.h"
#include "tests/UnitTest.h"

using namespace std;
using namespace eckit;
using namespace odb;

static void setUp()
{
	odb::Writer<> f("UnitTest.odb");
	odb::Writer<>::iterator it = f.begin();
    odb::MetaData& md = it->columns();

	md.setSize(4);
	it->setColumn(0, "lat@hdr", odb::REAL);
	it->setColumn(1, "lon@hdr", odb::REAL);
	it->setColumn(2, "obsvalue", odb::REAL);

    odb::BitfieldDef bfDef;
	bfDef.first.push_back("x");
	bfDef.second.push_back(1);
	bfDef.first.push_back("y");
	bfDef.second.push_back(2);

	it->setBitfieldColumn(3, "bf", odb::BITFIELD, bfDef);

	it->writeHeader();

	for (size_t i = 1; i <= 2; i++)
	{
		(*it)[0] = i; // col 0
		(*it)[1] = i; // col 1
		(*it)[2] = i; // col 2
		++it;
	}
}

static void selectIntoSecondFile()
{
	const string fileName = "UnitTest.odb";
	string sql = "select lat,lon,obsvalue,bf into \"UnitTest.odb\"";
	sql += " from \"" + fileName + "\" ;";

	odb::Select f(sql); //, fileName);
	odb::Select::iterator it = f.begin();

	//string c0 = it.columns()[0]->name();
	//string c1 = it.columns()[1]->name();
	//string c2 = it.columns()[2]->name();

	++it; // this is needed to push the second row to the INTO file 

	//Log::info()  << "c0=" << c0 << ", c1=" << c1 << ", c2=" << c2 << std::endl;
	///ASSERT("");
}

static void compareFiles()
{
	odb::Reader oda1("UnitTest.odb");
	odb::Reader oda2("UnitTest.odb");

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());
	
	odb::Comparator().compare(it1, end1, it2, end2, "UnitTest.odb", "UnitTest.odb");
}


static void test()
{
    selectIntoSecondFile();
    compareFiles();
}



static void tearDown(){}

TEST_MAIN;
