/// \file TestFunctionNorm.h
///
/// @author ECMWF, July 2010

#ifndef TEST_FUNCTION_NORM_H
#define TEST_FUNCTION_NORM_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionNorm : public TestCase {
public:
	TestFunctionNorm(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionNorm();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

