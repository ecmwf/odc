/// \file TestSelectDataHandle.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2010

#ifndef TestSelectDataHandle_H
#define TestSelectDataHandle_H

namespace odb {
namespace tool {
namespace test {

class TestSelectDataHandle : public TestCase {
public:
	TestSelectDataHandle(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestSelectDataHandle();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

