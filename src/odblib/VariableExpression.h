/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File VariableExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef VariableExpression_H
#define VariableExpression_H

//#include "odblib/SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class VariableExpression : public SQLExpression {
public:
	VariableExpression(const std::string&);
	VariableExpression(const VariableExpression&);
	~VariableExpression(); 

	SQLExpression* clone() const;

private:
// No copy allowed
	VariableExpression& operator=(const VariableExpression&);

	SQLExpression* value_;
	std::string name_;

// -- Overridden methods
	virtual void print(std::ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	const type::SQLType* type() const;
	virtual double eval(bool& missing) const;

	virtual bool isConstant() const;
	virtual bool isVector() const;
};

} // namespace expression
} // namespace sql
} // namespace odb

#endif
