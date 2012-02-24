/// \file TestMinMax.h
///
/// @author Piotr Kuchta, ECMWF, March 2009

#ifndef TestMinMax_H
#define TestMinMax_H

namespace odb {
namespace tool {
namespace test {

class TestMinMax : public TestCase {
public:
	TestMinMax(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestMinMax();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

