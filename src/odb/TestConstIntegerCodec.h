/// \file TestConstIntegerCodec.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#ifndef TestConstIntegerCodec_H
#define TestConstIntegerCodec_H

namespace odb {
namespace tool {
namespace test {

class TestConstIntegerCodec : public TestCase {
public:
	TestConstIntegerCodec(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestConstIntegerCodec();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

