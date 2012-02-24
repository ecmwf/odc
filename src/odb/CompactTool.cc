#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "Comparator.h"
#include "CompactTool.h"

namespace odb {
namespace tool {

ToolFactory<CompactTool> compact("compact");

CompactTool::CompactTool (int argc, char *argv[]) : Tool(argc, argv) { }

void CompactTool::run()
{
	if (parameters().size() != 3)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	PathName inFile = parameters(1);
	PathName outFile = parameters(2);

	odb::Reader in(inFile);
	odb::Writer<> out(outFile);

	odb::Reader::iterator it(in.begin());
	odb::Reader::iterator end(in.end());

	odb::Writer<>::iterator writer(out.begin());
	writer->pass1(it, end);
	
	odb::Reader outReader(outFile);
	Log::info() << "Verifying." << endl;	
	odb::Reader::iterator it1 = in.begin();
	odb::Reader::iterator end1 = in.end();

	odb::Reader::iterator it2 = outReader.begin();
	odb::Reader::iterator end2 = outReader.end();

	odb::Comparator comparator;
	comparator.compare(it1, end1, it2, end2, inFile, outFile);
}

} // namespace tool 
} // namespace odb 

