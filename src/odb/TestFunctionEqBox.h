/// \file TestFunctionEqBox.h
///
/// @author ECMWF, July 2010

#ifndef TEST_EQ_BOX_H
#define TEST_EQ_BOX_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionEqBox : public TestCase {
public:
	TestFunctionEqBox(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionEqBox();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

