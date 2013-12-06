/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSetvbuffer.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestSetvbuffer_H
#define TestSetvbuffer_H

#include "odblib/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestSetvbuffer : public TestCase {
public:
	TestSetvbuffer(int argc, char **argv);

	virtual void setUp();


	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestSetvbuffer();
private:
	void createFile(size_t numberOfColumns, long long numberOfRows, size_t);
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

