/// \file TestOrderBy.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#ifndef TestOrderBy_H
#define TestOrderBy_H

namespace odb {
namespace tool {
namespace test {

class TestOrderBy : public TestCase {
public:
	TestOrderBy(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestOrderBy();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

