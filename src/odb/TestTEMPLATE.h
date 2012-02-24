/// \file TestTEMPLATE.h
///
/// @author Piotr Kuchta, ECMWF, Jan 2011

#ifndef TestTEMPLATE_H
#define TestTEMPLATE_H

namespace odb {
namespace tool {
namespace test {

class TestTEMPLATE : public TestCase {
public:
	TestTEMPLATE(int argc, char **argv);
	virtual ~TestTEMPLATE();
	
	virtual void setUp();
	virtual void test();
	virtual void tearDown();

private:
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

