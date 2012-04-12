#include "odblib/oda.h"

#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "Comparator.h"
#include "SplitTool.h"

namespace odb {
namespace tool {

ToolFactory<SplitTool> split("split");

SplitTool::SplitTool (int argc, char *argv[]) : Tool(argc, argv) { }

void SplitTool::run()
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
	odb::DispatchingWriter out(outFile);

	odb::Reader::iterator it = in.begin();
	odb::Reader::iterator end = in.end();
	odb::DispatchingWriter::iterator outIt = out.begin();
	outIt->pass1(it, end);
}

} // namespace tool 
} // namespace odb 

