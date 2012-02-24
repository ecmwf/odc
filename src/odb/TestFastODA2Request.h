/// \file TestFastODA2Request.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestFastODA2Request_H
#define TestFastODA2Request_H

namespace odb {
namespace tool {
namespace test {

class TestFastODA2Request : public TestCase {
public:
	TestFastODA2Request(int argc, char **argv);
	virtual ~TestFastODA2Request();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

