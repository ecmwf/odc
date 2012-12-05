/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ShiftedBitColumnExpression.h
// Piotr Kuchta - ECMWF Dec 2012

#ifndef ShiftedBitColumnExpression_H
#define ShiftedBitColumnExpression_H

#include "odblib/BitColumnExpression.h"

namespace odb {
namespace sql {
namespace expression {

class ShiftedBitColumnExpression : public BitColumnExpression {
public:
	ShiftedBitColumnExpression(const string&, const string&, SQLTable*, int shift);
	ShiftedBitColumnExpression(const string&, const string&, const string&, int shift);
	~ShiftedBitColumnExpression(); 
private:
// No copy allowed
	ShiftedBitColumnExpression(const ShiftedBitColumnExpression&);
	ShiftedBitColumnExpression& operator=(const ShiftedBitColumnExpression&);

	int shift_;

// -- Overridden methods
	virtual void prepare(SQLSelect& sql);
	virtual double eval(bool& missing) const;
	virtual void print(ostream& s) const;

	friend ostream& operator<<(ostream& s, const ShiftedBitColumnExpression& p)
		{ p.print(s); return s; }
};

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
