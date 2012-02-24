/// \file TestMissingValue.h
///
/// @author Piotr Kuchta, ECMWF, Jun 2011

#ifndef TestMissingValue_H
#define TestMissingValue_H

namespace odb {
namespace tool {
namespace test {

class TestMissingValue : public TestCase {
public:
	TestMissingValue(int argc, char **argv);
	~TestMissingValue();

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

private:
	void selectIntoSecondFile();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

