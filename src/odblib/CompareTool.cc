#include "oda.h"
#include "Tool.h"
#include "ToolFactory.h"
#include "Comparator.h"
#include "CompareTool.h"
#include "StringTool.h"

#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {

ToolFactory<CompareTool> compare("compare");

CompareTool::CompareTool (int argc, char *argv[])
: Tool(argc, argv) 
{
	registerOptionWithArgument("-excludeColumnsTypes");
	if (parameters().size() != 3)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	ASSERT("First file does not exist!" && PathName(parameters()[1]).exist());
	ASSERT("Second file does not exist!" && PathName(parameters()[2]).exist());

	//ASSERT("First parameter is a directory name!" && !path1.isDir());
	//ASSERT("Second parameter is a directory name!" && !path2.isDir());

	file1 = new PathName(parameters()[1]);
	file2 = new PathName(parameters()[2]);
}


void CompareTool::run()
{
	Timer t(string("Comparing files ") + *file1 + " and " + *file2);
	odb::Reader oda1(*file1);
	odb::Reader oda2(*file2);

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());

	vector<string> excludedColumnsTypes = StringTool::split(",", optionArgument("-excludeColumnsTypes", string("")));

	Log::info() << "excludedColumnsTypes:" << excludedColumnsTypes << endl;
	
	bool checkMissing = ! optionIsSet("-dontCheckMissing");
	odb::Comparator(checkMissing).compare(it1, end1, it2, end2, *file1, *file2, excludedColumnsTypes);
}

} // namespace tool 
} // namespace odb 

