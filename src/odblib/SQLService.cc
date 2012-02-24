#include "SQLService.h"
#include "SQLUser.h"

namespace odb {
namespace sql {

SQLService::SQLService(int port):
	NetService(port)
{}

SQLService::~SQLService() {}


NetUser* SQLService::newUser(TCPSocket& protocol)
{
	return new SQLUser(protocol);
}

} // namespace sql
} // namespace odb
