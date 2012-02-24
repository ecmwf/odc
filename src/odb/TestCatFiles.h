/// \file TestCatFiles.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestCatFiles_H
#define TestCatFiles_H

namespace odb {
namespace tool {
namespace test {

class TestCatFiles : public TestCase {
public:
	TestCatFiles(int argc, char **argv);

	virtual void setUp();

	void testSelectIterator();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestCatFiles();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

