/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLServerOutput..h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLServerOutput_H
#define SQLServerOutput_H

#include "SQLOutput.h"
#include "TCPSocket.h"

namespace odb {
namespace sql {

class SQLServerSession;

class SQLServerOutput : public SQLOutput {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLServerOutput(SQLServerSession&,bool swapBytes);

// -- Destructor

	virtual ~SQLServerOutput(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods


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
	
	virtual void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	SQLServerOutput(const SQLServerOutput&);
	SQLServerOutput& operator=(const SQLServerOutput&);

// -- Members

	SQLServerSession& owner_;
	bool swapBytes_;
	unsigned long long count_;

	TCPSocket data_;

// -- Methods
	// None

// -- Overridden methods

	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const { NOTIMP; };
	virtual void outputDouble(double, bool) const { NOTIMP; };
	virtual void outputInt(double, bool) const { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) const { NOTIMP; };
	virtual void outputString(double, bool) const { NOTIMP; };
	virtual void outputBitfield(double, bool) const { NOTIMP; };
// -- Class members
	// None

// -- Class methods
	// None

// -- Friends


};

} // namespace sql
} // namespace odb

#endif
