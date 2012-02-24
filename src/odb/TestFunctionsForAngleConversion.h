/// \file TestFunctionsForAngleConversion.h
///
/// @author ECMWF, July 2010

#ifndef TEST_ANGLE_CONVERSION_H
#define TEST_ANGLE_CONVERSION_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionsForAngleConversion : public TestCase {
public:
	TestFunctionsForAngleConversion(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionsForAngleConversion();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

