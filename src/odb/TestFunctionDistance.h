/// \file TestFunctionDistance.h
///
/// @author ECMWF, July 2010

#ifndef TEST_DISTANCE_H
#define TEST_DISTANCE_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionDistance : public TestCase {
public:
	TestFunctionDistance(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionDistance();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

