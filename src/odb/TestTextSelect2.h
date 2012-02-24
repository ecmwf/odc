/// \file TestTextSelect2.h
///
/// @author Piotr Kuchta, ECMWF, Oct 2010

#ifndef TestTextSelect2_H
#define TestTextSelect2_H

namespace odb {
namespace tool {
namespace test {

class TestTextSelect2 : public TestCase {
public:
	TestTextSelect2(int argc, char **argv);
	~TestTextSelect2();

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

