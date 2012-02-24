/// \file TestTextSelect.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2010

#ifndef TestTextSelect_H
#define TestTextSelect_H

namespace odb {
namespace tool {
namespace test {

class TestTextSelect : public TestCase {
public:
	TestTextSelect(int argc, char **argv);
	~TestTextSelect();

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

