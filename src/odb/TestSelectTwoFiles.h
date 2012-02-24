/// \file TestSelectTwoFiles.h
///
/// @author Piotr Kuchta, ECMWF, Jun 2011

#ifndef TestSelectTwoFiles_H
#define TestSelectTwoFiles_H

namespace odb {
namespace tool {
namespace test {

class TestSelectTwoFiles : public TestCase {
public:
	TestSelectTwoFiles(int argc, char **argv);
	~TestSelectTwoFiles();

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

