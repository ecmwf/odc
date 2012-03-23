/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLService.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef SQLService_H
#define SQLService_H

#include "eclib/NetService.h"

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
