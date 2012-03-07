/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLServerSession.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLServerSession_H
#define SQLServerSession_H

#include "SQLSession.h"
#include "TCPServer.h"

namespace odb {
namespace sql {

class SQLServerSession : public SQLSession {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLServerSession(istream&,ostream&);

// -- Destructor

	~SQLServerSession(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void serve();
	TCPSocket& accept() { return data_.accept(); }

// -- Overridden methods


// -- Class members
	// None

// -- Class methods
	// None

protected:

// -- Members
	// None

// -- Methods
	
	// void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	SQLServerSession(const SQLServerSession&);
	SQLServerSession& operator=(const SQLServerSession&);

// -- Members

	TCPServer data_;

	istream& in_;
	ostream& out_;

	bool swap_;

	SQLStatement*          last_;
	map<int,SQLStatement*> prepared_;
	int next_;

// -- Methods
	// None

	void commandDO();
	void commandLOGIN();
	void commandPREPARE();
	void commandEXECUTE();
	void commandDESTROY();
	void commandPARAM();

	void parseStatement();

// -- Overridden methods

	virtual SQLOutput* defaultOutput();
	virtual void statement(SQLStatement *sql);


// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLServerSession& p)
	//	{ p.print(s); return s; }

};

} // namespace sql
} // namespace odb

#endif
