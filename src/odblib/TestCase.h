/*
 * (C) Copyright 1996-2012 ECMWF.
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

#include "odblib/Tool.h"

namespace odb {
namespace tool {
namespace test {

class TestCase : public Tool {
public:

	static void help(std::ostream &o) { o << "No help available for this command yet"; }
	static void usage(const string& name, std::ostream &o) { o << name << ": Not implemented yet"; }
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

#define TEST_FIXTURE(F, T) \
struct Test_##F##_##T : public F, public odb::tool::test::TestCase \
{ \
    Test_##F##_##T(int argc, char **argv) : F(), odb::tool::test::TestCase(argc, argv) {} \
    void test(); \
}; \
odb::tool::ToolFactory<Test_##F##_##T> test_##F##_##T(string("Test_") + #F + string("_") + #T); \
void Test_##F##_##T::test()

#define TEST(T) \
struct Test_##T : public odb::tool::test::TestCase \
{ \
    Test_##T(int argc, char **argv) : odb::tool::test::TestCase(argc, argv) {} \
    void test(); \
}; \
odb::tool::ToolFactory<Test_##T> test_##T(string("Test_") + #T); \
void Test_##T::test()

#define CHECK(expected) ASSERT(expected)
#define CHECK_EQUAL(expected, actual) ASSERT((expected) == (actual))

template <typename Expected, typename Actual>
bool CheckArrayEqual(const Expected& expected, const Actual& actual, const int count)
{
    bool equal = true;
    for (int i = 0; i < count; ++i)
        equal &= (expected[i] == actual[i]);
    return equal;
}

#define CHECK_ARRAY_EQUAL(expected, actual, count) \
ASSERT(odb::tool::test::CheckArrayEqual(expected, actual, count))

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

