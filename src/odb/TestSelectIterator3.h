/// \file TestSelectIterator3.h
///
/// @author Piotr Kuchta, ECMWF, April 2010

#ifndef TestSelectIterator3_H
#define TestSelectIterator3_H

namespace odb {
namespace tool {
namespace test {

class TestSelectIterator3 : public TestCase {
public:
	TestSelectIterator3(int argc, char **argv);

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	~TestSelectIterator3();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

