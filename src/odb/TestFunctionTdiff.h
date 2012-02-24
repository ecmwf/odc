/// \file TestFunctionTdiff.h
///
/// @author ECMWF, July 2010

#ifndef TEST_FUNCTION_TDIFF_H
#define TEST_FUNCTION_TDIFF_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionTdiff : public TestCase {
public:
	TestFunctionTdiff(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionTdiff();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

