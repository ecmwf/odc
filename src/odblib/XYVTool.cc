#include "oda.h"
#include "Tool.h"
#include "ToolFactory.h"
#include "XYVTool.h"

#include <iostream>
#include <fstream>
using namespace std;

namespace odb {
namespace tool {

ToolFactory<XYVTool> xyvTool("xyv");

XYVTool::XYVTool(int argc,char **argv): Tool(argc, argv) {}

XYVTool::~XYVTool() {}

void XYVTool::run()
{
	if (parameters().size() != 4)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;// 1;
	}

	PathName inputFile = parameters(1);
	string valueColumn = parameters(2);
	PathName outputFile = parameters(3);

	ofstream out;
	out.open(outputFile.c_str());

	out << "#GEO" << endl << endl;
	out << "#FORMAT XYV" << endl << endl;
	out << "PARAMETER = 12004" << endl << endl; 
	out << "x/long	y/lat	value" << endl;
	out << "#DATA" << endl << endl;

	string select = string("select lat, lon, ") + valueColumn + " from \"" + inputFile + "\";";
	Log::info() << select << endl;

	odb::Select oda(select);
	for (odb::Select::iterator it = oda.begin();
		it != oda.end();
		++it) 
	{
		out << (*it)[0] << "\t" << (*it)[1] << "\t" << (*it)[2] << endl;
	}
	out.close();
}

} // namespace tool 
} // namespace odb 

