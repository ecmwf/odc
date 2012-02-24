/// \file TestDecoding.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#ifndef TestDecoding_H
#define TestDecoding_H

namespace odb {
namespace tool {
namespace test {

class TestDecoding : public TestCase {
public:
	TestDecoding(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestDecoding();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

