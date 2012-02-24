/// \file TestFunctionCircle.h
///
/// @author ECMWF, July 2010

#ifndef TEST_CIRCLE_H
#define TEST_CIRCLE_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionCircle : public TestCase {
public:
	TestFunctionCircle(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionCircle();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

