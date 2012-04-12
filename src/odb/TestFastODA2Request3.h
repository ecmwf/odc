/// \file TestFastODA2Request3.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestFastODA2Request3_H
#define TestFastODA2Request3_H

namespace odb {
namespace tool {
namespace test {

class TestFastODA2Request3 : public TestCase {
public:
	TestFastODA2Request3(int argc, char **argv);
	virtual ~TestFastODA2Request3();
	
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

