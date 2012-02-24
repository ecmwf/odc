/// \file TestSimpleFilterIterator.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#ifndef TestSimpleFilterIterator_H
#define TestSimpleFilterIterator_H

namespace odb {
namespace tool {
namespace test {

class TestSimpleFilterIterator : public TestCase {
public:
	TestSimpleFilterIterator(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestSimpleFilterIterator();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

