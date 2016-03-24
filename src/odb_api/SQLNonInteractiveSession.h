/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLNonInteractiveSession.h
// Piotr Kuchta - ECMWF May 2015

#ifndef SQLNonInteractiveSession_H
#define SQLNonInteractiveSession_H

//#include "odb_api/SQLSession.h"
#include "SQLSession.h"

namespace odb {
namespace sql {

class SQLNonInteractiveSession : public SQLSession {
public:
    SQLNonInteractiveSession();
	~SQLNonInteractiveSession(); 

	SQLStatement* statement();
private:
// No copy allowed
	SQLNonInteractiveSession(const SQLNonInteractiveSession&);
	SQLNonInteractiveSession& operator=(const SQLNonInteractiveSession&);

    SQLStatement* statement_;

// -- Overridden methods
	void statement(SQLStatement*);
	SQLOutput* defaultOutput();

	//friend std::ostream& operator<<(std::ostream& s,const SQLNonInteractiveSession& p)
	//	{ p.print(s); return s; }

};

} // namespace sql 
} // namespace odb 

#endif
