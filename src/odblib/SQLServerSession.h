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
