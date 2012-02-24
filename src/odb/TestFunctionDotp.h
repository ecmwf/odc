/// \file TestFunctionDotp.h
///
/// @author ECMWF, July 2010

#ifndef TEST_FUNCTION_DOTP_H
#define TEST_FUNCTION_DOTP_H

namespace odb {
namespace tool {
namespace test {

class TestFunctionDotp : public TestCase {
public:
	TestFunctionDotp(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void testReaderIterator();

	virtual void tearDown();

	~TestFunctionDotp();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

