/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestFastODA2Request3.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestFastODA2Request3_H
#define TestFastODA2Request3_H

namespace odb {
namespace tool {
namespace test {

class TestFastODA2Request3 : public TestCase {
public:
	TestFastODA2Request3(int argc, char **argv);
	virtual ~TestFastODA2Request3();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
	eckit::Length createFile(const string& fileName, unsigned int andate, unsigned int antime, unsigned int reportype);

	eckit::Length size1_;
	eckit::Length size2_;
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

