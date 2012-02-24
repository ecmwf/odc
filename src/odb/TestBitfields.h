/// \file TestBitfields.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestBitfields_H
#define TestBitfields_H

namespace odb {
namespace tool {
namespace test {

class TestBitfields : public TestCase {
public:
	TestBitfields(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestBitfields();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

