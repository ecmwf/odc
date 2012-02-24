/// \file TestSQLFunctionsInfo.h
///
/// @author Piotr Kuchta, ECMWF, October 2009

#ifndef TestSQLFunctionsInfo_H
#define TestSQLFunctionsInfo_H

namespace odb {
namespace tool {
namespace test {

class TestSQLFunctionsInfo : public TestCase {
public:
	TestSQLFunctionsInfo(int argc, char **argv);
	virtual void test();
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif

