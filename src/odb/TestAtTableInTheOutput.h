/// \file TestAtTableInTheOutput.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestAtTableInTheOutput_H
#define TestAtTableInTheOutput_H

namespace odb {
namespace tool {
namespace test {

class TestAtTableInTheOutput : public TestCase {
public:
	TestAtTableInTheOutput(int argc, char **argv);
	~TestAtTableInTheOutput();

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

private:
	void selectIntoSecondFile();
	void compareFiles();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

