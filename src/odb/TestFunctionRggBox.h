/// \file TestFunctionRggBox.h
///
/// @author ECMWF, July 2010

#ifndef TEST_RGG_BOX_H
#define TEST_RGG_BOX_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionRggBox : public TestCase {
public:
	TestFunctionRggBox(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionRggBox();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

