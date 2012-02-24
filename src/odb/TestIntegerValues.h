/// \file TestIntegerValues.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestIntegerValues_H
#define TestIntegerValues_H

namespace odb {
namespace tool {
namespace test {

class TestIntegerValues : public TestCase {
public:
	TestIntegerValues(int argc, char **argv);
	virtual ~TestIntegerValues();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

