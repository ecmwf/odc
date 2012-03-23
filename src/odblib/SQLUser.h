/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLUser.h
// Baudouin Raoult - ECMWF Feb 02

#ifndef SQLUser_H
#define SQLUser_H

#include "eclib/NetUser.h"

namespace odb {
namespace sql {

class SQLUser : public NetUser {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLUser(TCPSocket&);

// -- Destructor

	~SQLUser(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

	// Uncomment for persistent, remove otherwise
	// static os_typespec* get_os_typespec();

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

	SQLUser(const SQLUser&);
	SQLUser& operator=(const SQLUser&);

// -- Members
	
// -- Methods
	

// -- Overridden methods
	// From NetUser

	virtual void serve(Stream&, istream&,ostream&);
	
// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLUser& p)
	//	{ p.print(s); return s; }

};

inline void destroy(SQLUser**) {}

} // namespace sql
} // namespace odb

#endif
