/// \file TestInMemoryDataHandle.h
///
/// @author Piotr Kuchta, ECMWF, April 2009

#ifndef TestInMemoryDataHandle_H
#define TestInMemoryDataHandle_H

namespace odb {
namespace tool {
namespace test {

class TestInMemoryDataHandle : public TestCase {
public:

	TestInMemoryDataHandle(int argc, char **argv);
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	virtual ~TestInMemoryDataHandle();

private:
	odb::InMemoryDataHandle	h;
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

