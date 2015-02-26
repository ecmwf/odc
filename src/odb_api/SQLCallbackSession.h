/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLCallbackSession.h
// Piotr Kuchta - ECMWF February 2015

#ifndef SQLCallbackSession_H
#define SQLCallbackSession_H

//#include "odb_api/SQLSession.h"
#include "SQLSession.h"

namespace odb {
namespace sql {

class SQLCallbackSession : public SQLSession {
public:
    SQLCallbackSession(std::ostream & = std::cout);
	~SQLCallbackSession(); 
    SQLStatement& statement();

private:
// No copy allowed
	SQLCallbackSession(const SQLCallbackSession&);
	SQLCallbackSession& operator=(const SQLCallbackSession&);

    std::ostream &out_;
    SQLStatement* sql_;

// -- Overridden methods
	void statement(SQLStatement*);

	SQLOutput* defaultOutput();
	//friend std::ostream& operator<<(std::ostream& s,const SQLCallbackSession& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
