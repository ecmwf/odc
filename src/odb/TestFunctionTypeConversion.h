/// \file TestFunctionTypeConversion.h
///
/// @author ECMWF, July 2010

#ifndef TEST_FUNCTION_TYPE_CONVERSION_H
#define TEST_FUNCTION_TYPE_CONVERSION_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionTypeConversion : public TestCase {
public:
	TestFunctionTypeConversion(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionTypeConversion();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

