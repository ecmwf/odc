/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestIntegerValues.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestIntegerValues_H
#define TestIntegerValues_H

#include "tests/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestIntegerValues : public TestCase {
public:
	TestIntegerValues(int argc, char **argv);
	virtual ~TestIntegerValues();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif


