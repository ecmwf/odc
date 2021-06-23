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
/// @author Piotr Kuchta, ECMWF, May 2009

#include "odc/core/MetaData.h"
#include "odc/Select.h"

#include "TestCase.h"


using namespace std;
using namespace eckit;
using namespace odc;

/// UnitTest syntax: select *@odb_table from "file.oda";
///
static void test()
{
    const std::string SELECT = "select *@hdr from \"2000010106-reduced.odb\";";

	odc::Select oda(SELECT);

	odc::Select::iterator it = oda.begin();
	ASSERT("hdr has 27 columns excluding @LINKs." && it->columns().size() == 27);
}



static void setUp(){}
static void tearDown(){}

SIMPLE_TEST(Star)
