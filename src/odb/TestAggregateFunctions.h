/// \file TestAggregateFunctions.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#ifndef TestAggregateFunctions_H
#define TestAggregateFunctions_H

namespace odb {
namespace tool {
namespace test {

class TestAggregateFunctions : public TestCase {
public:
	TestAggregateFunctions(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAggregateFunctions();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

