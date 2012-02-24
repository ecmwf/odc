/// \file TestSelectIterator2.h
///
/// @author Piotr Kuchta, ECMWF, April 2010

#ifndef TestSelectIterator2_H
#define TestSelectIterator2_H

namespace odb {
namespace tool {
namespace test {

class TestSelectIterator2 : public TestCase {
public:
	TestSelectIterator2(int argc, char **argv);

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	~TestSelectIterator2();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

