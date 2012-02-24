#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "SQLServer.h"
#include "SQLServerTool.h"

namespace odb {
namespace tool {

ToolFactory<SQLServerTool> sqlServerTool("server");

SQLServerTool::SQLServerTool(int argc, char **argv): Tool(argc,argv) {}

SQLServerTool::~SQLServerTool() {}

void SQLServerTool::run()
{
	int port = Resource<long>("-port", 0);
	if(port == 0)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	Log::info() << "Starting server on port " << port << endl;
	odb::sql::SQLServer server(port);
	::pause();
	Log::info() << "Server says good bye on port " << port << endl;
	return;
}

} // namespace tool 
} // namespace odb 

