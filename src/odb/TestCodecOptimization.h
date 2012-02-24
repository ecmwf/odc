/// \file TestCodecOptimization.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestCodecOptimization_H
#define TestCodecOptimization_H

namespace odb {
namespace tool {
namespace test {

class TestCodecOptimization : public TestCase {
public:
	TestCodecOptimization(int argc, char **argv);

	virtual void setUp();

	void testSelectIterator();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestCodecOptimization();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

