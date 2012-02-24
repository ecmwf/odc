// File SQLDouble.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLDouble_H
#define SQLDouble_H

#include "SQLType.h"

namespace odb {
namespace sql {

class SQLOutput;

namespace type {

class SQLDouble : public SQLType {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLDouble(const string& );

// -- Destructor

	~SQLDouble(); // Change to virtual if base class

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

	SQLDouble(const SQLDouble&);
	SQLDouble& operator=(const SQLDouble&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

	virtual size_t size() const;
	virtual int getKind() const { return doubleType; }

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLDouble& p)
	//	{ p.print(s); return s; }

};

} // namespace type 
} // namespace sql
} // namespace odb

#endif
