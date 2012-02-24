/// \file TestAggregateFunctions2.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#ifndef TestAggregateFunctions2_H
#define TestAggregateFunctions2_H

namespace odb {
namespace tool {
namespace test {

class TestAggregateFunctions2 : public TestCase {
public:
	TestAggregateFunctions2(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAggregateFunctions2();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

