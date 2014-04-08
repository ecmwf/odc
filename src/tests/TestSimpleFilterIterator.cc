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
/// @author Piotr Kuchta, ECMWF, June 2009

#include "eckit/log/Timer.h"
#include "odblib/Select.h"
#include "odblib/Reader.h"
#include "odblib/SimpleFilter.h"

#include "eckit/testing/UnitTest.h"


using namespace std;
using namespace eckit;
using namespace odb;

//int const OBSTYPE = 1;
//int const SENSOR = 24;


/// UnitTest DispatchingWriter
///
static void test()
{
	const string fileName = "2000010106.odb";
	string sql = std::string("select * from \"") + fileName + "\" where obstype = 7;";
	long n0 = 0;
	long n1 = 0;
	long n2 = 0;

	{
		Timer t("test: selecting rows where obstype == 7 the simple way");
		odb::Reader oda(fileName);
		odb::Reader::iterator it = oda.begin();
		size_t obstype_index = it->columns().columnIndex("obstype");
		for (; it != oda.end(); ++it)
		{
			if ((*it)[obstype_index] == 7.0)
			{
				++n0;
#if 0
				if (n0 < 10 || n0 > 3134386 - 3)
				{
					cerr << "" << n0 << ": "; 
					for (size_t i=30; i < 40; ++i)
						cerr << it[i] << ", ";
					cerr << std::endl;
				}
#endif
			}
		}
	}

	{
		Timer t("test: selecting rows where obstype == 7");

		odb::Reader oda(fileName);

		typedef odb::SimpleFilter<odb::Reader::iterator> Filter;
		Filter simpleFilter(oda.begin(), oda.end(), "obstype", 7.0);
		Filter::iterator sIt = simpleFilter.begin();
		for (; sIt != simpleFilter.end(); ++sIt)
		{
			++n1;
#if 0
			if (n1 < 10 || n1 > 3134386 - 3)
			{
				cerr << "" << n1 << ": "; 
				for (size_t i=30; i < 40; ++i)
					cerr << sIt[i] << ", ";
				cerr << std::endl;
			}
#endif
		}

		Log::info() << "test: selected " << n1 << " rows." << std::endl;
	}

	{
		Timer t("test: selecting rows using SQL where obstype == 7");
		Log::info() << "test: Execute '" << sql << "'" << std::endl;

		odb::Select odas(sql, fileName);
		for(odb::Select::iterator it = odas.begin(); it != odas.end(); ++it)
		{
			++n2;

#if 0
			if (n2 < 10 || n2 > 3134386 - 3)
			{
				cerr << "" << n2 << ": ";
				for (size_t i=30; i < 40; ++i)
					cerr << it[i] << ", ";
				cerr << std::endl;
			}
#endif
		}

		Log::info() << "test: selected " << n2 << " rows." << std::endl;
	}

	Log::info() << "test: n0=" << n0 << ", n1=" << n1 << ", n2=" << n2 << std::endl;
	ASSERT(n0 == n1);
	ASSERT(n1 == n2);
}



static void setUp(){}
static void tearDown(){}

RUN_SIMPLE_TEST
