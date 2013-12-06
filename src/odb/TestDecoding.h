/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestDecoding.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#ifndef TestDecoding_H
#define TestDecoding_H

#include "odblib/TestCase.h"

namespace odb {
namespace tool {
namespace test {

#include "odblib/TestCase.h"


class TestDecoding : public TestCase {
public:
	TestDecoding(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestDecoding();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

