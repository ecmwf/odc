/// \file TestAAAImportODBDispatching.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestAAAImportODBDispatching_H
#define TestAAAImportODBDispatching_H

namespace odb {
namespace tool {
namespace test {

class TestAAAImportODBDispatching : public TestCase {
public:
	TestAAAImportODBDispatching(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAAAImportODBDispatching();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

