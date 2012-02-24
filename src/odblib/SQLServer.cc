#include "SQLServer.h"
#include "SQLService.h"

namespace odb {
namespace sql {

SQLServer::SQLServer(int port):
	odb_(new SQLService(port))
{
	odb_.start();
}

SQLServer::~SQLServer()
{
	odb_.stop();
}

} // namespace sql 
} // namespace odb 
