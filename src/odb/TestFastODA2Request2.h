/// \file TestFastODA2Request2.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestFastODA2Request2_H
#define TestFastODA2Request2_H

namespace odb {
namespace tool {
namespace test {

class TestFastODA2Request2 : public TestCase {
public:
	TestFastODA2Request2(int argc, char **argv);
	virtual ~TestFastODA2Request2();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
	Length createFile(const string& fileName, unsigned int andate, unsigned int antime, unsigned int reportype);

	Length size1_;
	Length size2_;
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

