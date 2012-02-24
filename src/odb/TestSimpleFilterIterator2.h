/// \file TestSimpleFilterIterator2.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#ifndef TestSimpleFilterIterator2_H
#define TestSimpleFilterIterator2_H

namespace odb {
namespace tool {
namespace test {

class TestSimpleFilterIterator2 : public TestCase {
public:
	TestSimpleFilterIterator2(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestSimpleFilterIterator2();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

