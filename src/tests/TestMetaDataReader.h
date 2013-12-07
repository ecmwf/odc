/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestMetaDataReader.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestMetaDataReader_H
#define TestMetaDataReader_H

#include "tests/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestMetaDataReader : public TestCase {
public:
	TestMetaDataReader(int argc, char **argv);
	virtual ~TestMetaDataReader();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif


