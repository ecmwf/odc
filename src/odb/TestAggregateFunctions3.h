/// \file TestAggregateFunctions3.h
///
/// @author Piotr Kuchta, ECMWF, September 2010

#ifndef TestAggregateFunctions3_H
#define TestAggregateFunctions3_H

namespace odb {
namespace tool {
namespace test {

class TestAggregateFunctions3 : public TestCase {
public:
	TestAggregateFunctions3(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAggregateFunctions3();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

