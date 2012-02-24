// File SQLCountStar.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLCountStar_H
#define SQLCountStar_H

#include "SQLStatement.h"

namespace odb {
namespace sql {

class SQLCountStar : public SQLStatement {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLCountStar(const string& table);

// -- Destructor

	~SQLCountStar(); // Change to virtual if base class

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

protected:

// -- Members
	// None

// -- Methods
	
	void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	SQLCountStar(const SQLCountStar&);
	SQLCountStar& operator=(const SQLCountStar&);

// -- Members

	string table_;

// -- Methods
	// None

// -- Overridden methods

	virtual unsigned long long execute(SQLDatabase&);

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLCountStar& p)
	//	{ p.print(s); return s; }

};

} // namespace sql 
} // namespace odb 

#endif
