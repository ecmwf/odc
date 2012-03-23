/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file Expressions.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef Expressions_H
#define Expressions_H

#include "eclib/machine.h"

#include "eclib/Exceptions.h"

#include "odblib/SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

typedef vector<odb::sql::expression::SQLExpression*> ExpressionsVector;

class Expressions : public SQLExpression, public ExpressionsVector
{
public:
	Expressions() : ExpressionsVector() {}
	Expressions(size_t i) : ExpressionsVector(i) {}
	Expressions(size_t i, SQLExpression* e) : ExpressionsVector(i, e) {}

	Expressions(const Expressions& e)
	: SQLExpression(), ExpressionsVector(e)
	{}

	Expressions& operator=(const Expressions&);

	virtual void release();

	virtual void print(ostream& s) const;

	friend ostream& operator<<(ostream& o, const Expressions& e)
		{ e.print(o); return o; }

//////////////////////////////////////////////////////////////////////////////////////

	virtual void prepare(SQLSelect&)  {}
	virtual void cleanup(SQLSelect&)  {}

	// -- For WHERE
	virtual double eval(bool& missing) const  { NOTIMP; }

	virtual bool isConstant() const  { NOTIMP; }
	virtual bool isNumber() const { return false; }
	virtual bool isVector() const { return true; }
	virtual Vector& vector() { return *this; }

	virtual SQLExpression* simplify(bool&) { return this; }

	virtual SQLExpression* clone() const;
	
	virtual bool isAggregate() const { return false; }
	// For select expression

	virtual void output(SQLOutput&) const { return NOTIMP; }
	virtual void partialResult() {}
	virtual void expandStars(const std::vector<SQLTable*>&,expression::Expressions&) { NOTIMP; }
//////////////////////////////////////////////////////////////////////////////////////
};

} // namespace expression
} // namespace sql
} // namespace odb

#endif
