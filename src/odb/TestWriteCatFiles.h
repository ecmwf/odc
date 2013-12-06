/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestWriteCatFiles.h
///
/// @author Piotr Kuchta, ECMWF, Jun 2011

#ifndef TestWriteCatFiles_H
#define TestWriteCatFiles_H

#include "odblib/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestWriteCatFiles : public TestCase {
public:
	TestWriteCatFiles(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestWriteCatFiles();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

