/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestConstCodec.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#ifndef TestConstCodec_H
#define TestConstCodec_H

#include "tests/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestConstCodec : public TestCase {
public:
	TestConstCodec(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestConstCodec();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif


