/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionTHIN.h
/// ECMWF July 2010

#ifndef FunctionTHIN_H
#define FunctionTHIN_H

#include "odblib/FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionTHIN : public FunctionExpression {
public:
	FunctionTHIN(const string&, const expression::Expressions&);
	FunctionTHIN(const FunctionTHIN&);
	~FunctionTHIN(); // Change to virtual if base class

	SQLExpression* clone() const;

// -- Overridden methods
	virtual const odb::sql::type::SQLType* type() const;
	virtual void output(ostream& s) const;
protected:
// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual bool isConstant() const;
	virtual double eval(bool& missing) const;
	virtual SQLExpression* simplify(bool&);
	bool isAggregate() const { return false; }

private:
// No copy allowed
	FunctionTHIN& operator=(const FunctionTHIN&);

	unsigned long long* count_;
// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionTHIN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
