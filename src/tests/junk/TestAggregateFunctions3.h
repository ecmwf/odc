/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAggregateFunctions3.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#ifndef TestAggregateFunctions3_H
#define TestAggregateFunctions3_H

#include "tests/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestAggregateFunctions3 : public TestCase {
public:
	TestAggregateFunctions3(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAggregateFunctions3();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif


