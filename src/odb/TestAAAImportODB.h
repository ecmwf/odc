/// \file TestAAAImportODB.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestAAAImportODB_H
#define TestAAAImportODB_H

namespace odb {
namespace tool {
namespace test {

class TestAAAImportODB : public TestCase {
public:
	TestAAAImportODB(int argc, char **argv);

	virtual void setUp();

	virtual void test();

	virtual void tearDown();

	~TestAAAImportODB();
};

} // namespace test 
} // namespace tool 
} // namespace odb 

#endif

