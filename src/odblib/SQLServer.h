// File SQLServer.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef SQLServer_H
#define SQLServer_H

#include "ThreadControler.h"

namespace odb {
namespace sql {

class SQLServer {
public:

// -- Contructors

	SQLServer(int port);

// -- Destructor

	~SQLServer();

private:

// No copy allowed

	SQLServer(const SQLServer&);
	SQLServer& operator=(const SQLServer&);

// -- Members

	ThreadControler odb_;

};

} // namespace sql
} // namespace odb

#endif
