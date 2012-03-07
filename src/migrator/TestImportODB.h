/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestImportODB.h
///
/// @author Piotr Kuchta, ECMWF, March 2009

#ifndef TestImportODB_H
#define TestImportODB_H

namespace odb {
namespace tool {

class TestImportODB : public test::TestCase {
public:
	TestImportODB(int argc, char **argv);

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	~TestImportODB();
};

} // namespace tool 
} // namespace odb 

#endif

