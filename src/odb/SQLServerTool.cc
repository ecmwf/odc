/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

