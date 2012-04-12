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

