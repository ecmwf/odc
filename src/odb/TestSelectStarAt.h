/// \file TestSelectStarAt.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestSelectStarAt_H
#define TestSelectStarAt_H

namespace odb {
namespace tool {
namespace test {

class TestSelectStarAt : public TestCase {
public:
	TestSelectStarAt(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestSelectStarAt();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

