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
