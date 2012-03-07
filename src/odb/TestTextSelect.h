/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestTextSelect.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2010

#ifndef TestTextSelect_H
#define TestTextSelect_H

namespace odb {
namespace tool {
namespace test {

class TestTextSelect : public TestCase {
public:
	TestTextSelect(int argc, char **argv);
	~TestTextSelect();

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
	void selectStarOneColumn();
	void selectSumOneColumn();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

