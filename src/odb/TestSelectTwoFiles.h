/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectTwoFiles.h
///
/// @author Piotr Kuchta, ECMWF, Jun 2011

#ifndef TestSelectTwoFiles_H
#define TestSelectTwoFiles_H

#include "odblib/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestSelectTwoFiles : public TestCase {
public:
	TestSelectTwoFiles(int argc, char **argv);
	~TestSelectTwoFiles();

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

