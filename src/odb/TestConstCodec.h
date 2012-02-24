/// \file TestConstCodec.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#ifndef TestConstCodec_H
#define TestConstCodec_H

namespace odb {
namespace tool {
namespace test {

class TestConstCodec : public TestCase {
public:
	TestConstCodec(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestConstCodec();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

