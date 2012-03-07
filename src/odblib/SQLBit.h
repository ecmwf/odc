/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLBit.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBit_H
#define SQLBit_H

#include "SQLType.h"

namespace odb {
namespace sql {
namespace type {

class SQLBit : public SQLType {
public:
	SQLBit(const string& ,unsigned long ,unsigned long );
	~SQLBit(); // Change to virtual if base class

	unsigned long mask()  const { return mask_; }
	unsigned long shift() const { return shift_; }

private:
// No copy allowed
	SQLBit(const SQLBit&);
	SQLBit& operator=(const SQLBit&);

	unsigned long mask_;
	unsigned long shift_;

// -- Overridden methods
	// None

	virtual size_t size() const;
	void output(SQLOutput&, double, bool) const;
	virtual int getKind() const { return integerType; }

	//friend ostream& operator<<(ostream& s,const SQLBit& p)
	//	{ p.print(s); return s; }
};

} // namespace type
} // namespace sql 
} // namespace odb 

#endif
