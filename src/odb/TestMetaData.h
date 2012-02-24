/// \file TestMetaData.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestMetaData_H
#define TestMetaData_H

namespace odb {
namespace tool {
namespace test {

class TestMetaData : public TestCase {
public:
	TestMetaData(int argc, char **argv);
	virtual ~TestMetaData();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

