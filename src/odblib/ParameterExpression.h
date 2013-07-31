/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ParameterExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef ParameterExpression_H
#define ParameterExpression_H

#include "odblib/SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class ParameterExpression : public SQLExpression {
public:
	ParameterExpression(int);
	ParameterExpression(const ParameterExpression&);
	~ParameterExpression(); 

	SQLExpression* clone() const;

private:
// No copy allowed
	ParameterExpression& operator=(const ParameterExpression&);

// -- Members
	// None
	double value_;
	int    which_;

	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	virtual double eval(bool& missing) const;
	const type::SQLType* type() const;
	virtual bool isConstant() const;
};

} // namespace expression 
} // namespace sql 
} // namespace odb 

#endif
