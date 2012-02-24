// File SQLService.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef SQLService_H
#define SQLService_H

#include "NetService.h"

namespace odb {
namespace sql {

class SQLService : public NetService {
public:

// -- Contructors

	SQLService(int);

// -- Destructor

	~SQLService();

private:

// No copy allowed

	SQLService(const SQLService&);
	SQLService& operator=(const SQLService&);

// -- Overridden methods

	// From NetService

	virtual NetUser* newUser(TCPSocket&);
	virtual string   name() { return "sql"; }

};

} // namespace sql
} // namespace odb

#endif
