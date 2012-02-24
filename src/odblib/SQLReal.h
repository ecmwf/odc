// File SQLReal.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLReal_H
#define SQLReal_H

#include "SQLType.h"

namespace odb {
namespace sql {

class SQLOutput;

namespace type {

class SQLReal : public SQLType {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLReal(const string& );

// -- Destructor

	~SQLReal(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

// -- Overridden methods
	virtual void output(SQLOutput&, double, bool) const;

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

	SQLReal(const SQLReal&);
	SQLReal& operator=(const SQLReal&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

	virtual size_t size() const;
	virtual int getKind() const { return realType; }

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLReal& p)
	//	{ p.print(s); return s; }

};

} // namespace type 
} // namespace sql
} // namespace odb

#endif
