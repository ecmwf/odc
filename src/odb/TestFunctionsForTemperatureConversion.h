/// \file TestFunctionsForTemperatureConversion.h
///
/// @author ECMWF, July 2010

#ifndef TEST_TEMPERATURE_CONVERSION_H
#define TEST_TEMPERATURE_CONVERSION_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionsForTemperatureConversion : public TestCase {
public:
	TestFunctionsForTemperatureConversion(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionsForTemperatureConversion();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

