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

#include "odblib/MetaData.h"
#include "odblib/Select.h"

#include "UnitTest.h"

namespace odb {
namespace tool {
namespace test {



/// UnitTest syntax: select *@odb_table from "file.oda";
///
void UnitTest::test()
{
    const std::string SELECT = "select *@hdr from \"2000010106.odb\";";

	odb::Select oda(SELECT);

	odb::Select::iterator it = oda.begin();
	ASSERT("hdr has 27 columns excluding @LINKs." && it->columns().size() == 27);
}

void UnitTest::setUp() {}

void UnitTest::tearDown() {}

} // namespace test
} // namespace tool 
} // namespace odb 



