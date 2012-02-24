/// \file TestStar.h
///
/// @author Piotr Kuchta, ECMWF, May 2009

#ifndef TestStar_H
#define TestStar_H

namespace odb {
namespace tool {
namespace test {

class TestStar : public TestCase {
public:
	TestStar(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestStar();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

