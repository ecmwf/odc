/// \file TestSQLFunctionsInfo.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "Log.h"
#include "Tool.h"
#include "TestCase.h"
#include "FunctionExpression.h"
#include "TestSQLFunctionsInfo.h"
#include "ToolFactory.h"

namespace odb {
namespace tool {
namespace test {

ToolFactory<TestSQLFunctionsInfo> _TestSQLFunctionsInfo("TestSQLFunctionsInfo");

TestSQLFunctionsInfo::TestSQLFunctionsInfo(int argc, char **argv)
: TestCase(argc, argv)
{}

void TestSQLFunctionsInfo::test()
{
	typedef vector<pair<string, int> > FI;

	FI& functionsInfo = odb::sql::expression::function::FunctionFactory::functionsInfo();

	Log::info() << "FunctionFactory::functionsInfo().size() == " << functionsInfo.size() << endl;
	for (FI::iterator i = functionsInfo.begin(); i != functionsInfo.end(); ++i)
	{
		Log::info() << i->first << "/" << i->second;
		if (i + 1 != functionsInfo.end())
			Log::info() << ", ";
	}
	Log::info() << endl;
}

} // namespace test
} // namespace tool 
} // namespace odb 

