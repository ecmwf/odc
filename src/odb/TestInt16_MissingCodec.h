/// \file TestInt16_MissingCodec.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2010

#ifndef TestInt16_MissingCodec_H
#define TestInt16_MissingCodec_H

namespace odb {
namespace tool {
namespace test {

class TestInt16_MissingCodec : public TestCase {
public:
	TestInt16_MissingCodec(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	/// TODO: Delete the test file.
	virtual void tearDown();

	~TestInt16_MissingCodec();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

