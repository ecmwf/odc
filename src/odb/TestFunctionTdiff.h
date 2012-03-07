/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFunctionTdiff.h
///
/// @author ECMWF, July 2010

#ifndef TEST_FUNCTION_TDIFF_H
#define TEST_FUNCTION_TDIFF_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionTdiff : public TestCase {
public:
	TestFunctionTdiff(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionTdiff();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

