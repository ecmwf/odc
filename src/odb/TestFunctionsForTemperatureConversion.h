/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionsForTemperatureConversion.h
///
/// @author ECMWF, July 2010

#ifndef TEST_TEMPERATURE_CONVERSION_H
#define TEST_TEMPERATURE_CONVERSION_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionsForTemperatureConversion : public TestCase {
public:
	TestFunctionsForTemperatureConversion(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionsForTemperatureConversion();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

