#include "oda.h"
#include "Tool.h"
#include "ToolFactory.h"
#include "Comparator.h"
#include "CompareTool.h"

#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {

ToolFactory<CompareTool> compare("compare");

CompareTool::CompareTool (int argc, char *argv[])
: Tool(argc, argv) 
{
	if (argc != 3)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}
	
	PathName path1(argv[1]);
	PathName path2(argv[1]);

    ASSERT("First file does not exist!" && path1.exists());
    ASSERT("Second file does not exist!" && path2.exists());

	//ASSERT("First parameter is a directory name!" && !path1.isDir());
	//ASSERT("Second parameter is a directory name!" && !path2.isDir());

	file1 = new PathName(argv[1]);
	file2 = new PathName(argv[2]);
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
	
	comparator.compare(it1, end1, it2, end2, *file1, *file2);
}

} // namespace tool 
} // namespace odb 

