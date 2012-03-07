/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestAAAImportODB.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestAAAImportODB_H
#define TestAAAImportODB_H

namespace odb {
namespace tool {
namespace test {

class TestAAAImportODB : public TestCase {
public:
	TestAAAImportODB(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAAAImportODB();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

