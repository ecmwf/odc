/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTEMPLATE.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestTEMPLATE_H
#define TestTEMPLATE_H

#include "tests/TestCase.h"

namespace odb {
namespace tool {
namespace test {

class TestTEMPLATE : public TestCase {
public:
	TestTEMPLATE(int argc, char **argv);
	virtual ~TestTEMPLATE();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif


