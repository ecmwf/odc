/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestCase.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestCase_H
#define TestCase_H

namespace odb {
namespace tool {
namespace test {

class TestCase : public Tool {
public:

	static void help(ostream &o) { o << "No help available for this command yet"; }
	static void usage(const string& name, ostream &o) { o << name << ": Not implemented yet"; }
	virtual void run();

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	virtual ~TestCase();

protected:
	TestCase(int argc, char **argv);
};

typedef vector<TestCase*> TestCases;

#define TESTCASE(F) \
struct Test_##F : public TestCase { Test_##F(int argc, char **argv) : TestCase(argc, argv) {} void test() { F(); } }; \
ToolFactory<Test_##F> test_##F(string("Test_") + #F); 

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

