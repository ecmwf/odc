/// \file TestDistinct.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#ifndef TestDistinct_H
#define TestDistinct_H

namespace odb {
namespace tool {
namespace test {

class TestDistinct : public TestCase {
public:
	TestDistinct(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestDistinct();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

