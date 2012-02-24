/// \file TestImportODB.h
///
/// @author Piotr Kuchta, ECMWF, March 2009

#ifndef TestImportODB_H
#define TestImportODB_H

namespace odb {
namespace tool {

class TestImportODB : public test::TestCase {
public:
	TestImportODB(int argc, char **argv);

	virtual void setUp();
	virtual void test();
	virtual void tearDown();

	~TestImportODB();
};

} // namespace tool 
} // namespace odb 

#endif

