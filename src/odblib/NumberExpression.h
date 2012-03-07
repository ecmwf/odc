/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File NumberExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef NumberExpression_H
#define NumberExpression_H

#include "SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class NumberExpression : public SQLExpression {
public:
	NumberExpression(double value);
	NumberExpression(const NumberExpression&);
	~NumberExpression();

	SQLExpression* clone() const;

	void value(double v) { value_ = v; }

private:
// No copy allowed
	NumberExpression& operator=(const NumberExpression&);

	double value_;

// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	virtual double eval(bool& missing) const;
	virtual bool isConstant() const { return true; }
	virtual bool isNumber() const { return true; }
};

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
