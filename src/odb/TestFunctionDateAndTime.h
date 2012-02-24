/// \file TestFunctionDateAndTime.h
///
/// @author ECMWF, July 2010

#ifndef TEST_FUNCTION_DATE_AND_TIME_H
#define TEST_FUNCTION_DATE_AND_TIME_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionDateAndTime : public TestCase {
public:
	TestFunctionDateAndTime(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionDateAndTime();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

