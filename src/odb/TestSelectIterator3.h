/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestSelectIterator3.h
///
/// @author Piotr Kuchta, ECMWF, April 2010

#ifndef TestSelectIterator3_H
#define TestSelectIterator3_H

namespace odb {
namespace tool {
namespace test {

class TestSelectIterator3 : public TestCase {
public:
	TestSelectIterator3(int argc, char **argv);

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	~TestSelectIterator3();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

