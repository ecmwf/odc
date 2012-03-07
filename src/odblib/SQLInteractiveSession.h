/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLInteractiveSession.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLInteractiveSession_H
#define SQLInteractiveSession_H

#include "SQLSession.h"

namespace odb {
namespace sql {

class SQLInteractiveSession : public SQLSession {
public:

// -- Exceptions
	// None

// -- Constructors

	SQLInteractiveSession(ostream & = cout);

// -- Destructor

	~SQLInteractiveSession(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

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

	SQLInteractiveSession(const SQLInteractiveSession&);
	SQLInteractiveSession& operator=(const SQLInteractiveSession&);

// -- Members
	ostream &out_;

// -- Methods
	// None

// -- Overridden methods

	void       statement(SQLStatement*);
	SQLOutput* defaultOutput();

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLInteractiveSession& p)
	//	{ p.print(s); return s; }

};

} // namespace sql 
} // namespace odb 

#endif
