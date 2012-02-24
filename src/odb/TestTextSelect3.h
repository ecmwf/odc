/// \file TestTextSelect3.h
///
/// @author Piotr Kuchta, ECMWF, Oct 2010

#ifndef TestTextSelect3_H
#define TestTextSelect3_H

namespace odb {
namespace tool {
namespace test {

class TestTextSelect3 : public TestCase {
public:
	TestTextSelect3(int argc, char **argv);
	~TestTextSelect3();

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
	void selectStarOneColumn();
	void selectSumOneColumn();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

