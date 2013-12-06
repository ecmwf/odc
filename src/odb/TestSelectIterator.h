/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectIterator.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestSelectIterator_H
#define TestSelectIterator_H

#include "odblib/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestSelectIterator : public TestCase {
public:
	TestSelectIterator(int argc, char **argv);

	/// Writes 10 rows to file test.oda.
	virtual void setUp();

	void testBug01();

	/// Reads 10 rows using ReaderIterator and for_each.
	void testReaderIteratorForEach();

	/// Reads 10 rows using ReaderIterator in a loop.
	void testReaderIteratorLoop();

	/// Reads 10 rows using SelectIterator and for_each
	void testSelectIteratorForEach();

	/// Reads 10 rows using SelectIterator in a loop.
	void testSelectIteratorLoop();


	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestSelectIterator();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

