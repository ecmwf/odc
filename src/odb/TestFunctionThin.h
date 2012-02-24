/// \file TestFunctionThin.h
///
/// @author ECMWF, July 2010

#ifndef TEST_THIN_H
#define TEST_THIN_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionThin : public TestCase {
public:
	TestFunctionThin(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionThin();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

