/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAtTableInTheOutput.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestAtTableInTheOutput_H
#define TestAtTableInTheOutput_H

namespace odb {
namespace tool {
namespace test {

#include "odblib/TestCase.h"

class TestAtTableInTheOutput : public TestCase {
public:
	TestAtTableInTheOutput(int argc, char **argv);
	~TestAtTableInTheOutput();

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

private:
	void selectIntoSecondFile();
	void compareFiles();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

