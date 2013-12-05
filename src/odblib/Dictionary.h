/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file Dictionary.h
/// Piotr Kuchta - ECMWF Dec 11

#ifndef Dictionary_H
#define Dictionary_H

//#include "eckit/eckit.h"

//#include "eckit/exception/Exceptions.h"

//#include "odblib/SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

typedef std::map<std::string,odb::sql::expression::SQLExpression*> Map;

class Dictionary : public SQLExpression, public Map
{
public:
	Dictionary() : Map() {}
	Dictionary(const Dictionary& e)
	: SQLExpression(), Map(e)
	{}

	Dictionary& operator=(const Dictionary&);

	virtual void release();

	virtual void print(std::ostream& s) const;

	friend std::ostream& operator<<(std::ostream& o, const Dictionary& e)
		{ e.print(o); return o; }

//////////////////////////////////////////////////////////////////////////////////////
	
	virtual const odb::sql::type::SQLType* type() const;

	virtual void prepare(SQLSelect&)  { NOTIMP; }
	virtual void cleanup(SQLSelect&)  { NOTIMP; }

	// -- For WHERE
	virtual double eval(bool& missing) const  { NOTIMP; }

	virtual bool isConstant() const  { NOTIMP; }
	virtual bool isNumber() const { return false; }
	virtual bool isVector() const { return false; }
	//virtual const Vector& std::vector() const { return *this; }
	virtual bool isDictionary() const { return true; }
	virtual Dictionary& dictionary() { return *this; }

	virtual SQLExpression* simplify(bool&) { NOTIMP; }

	virtual SQLExpression* clone() const;
	
	virtual bool isAggregate() const { return false; }
	// For select expression

	virtual void output(SQLOutput&) const { return NOTIMP; }
	virtual void partialResult() {}
	virtual void expandStars(const std::vector<SQLTable*>&,expression::Dictionary&) { NOTIMP; }
//////////////////////////////////////////////////////////////////////////////////////
};

} // namespace expression
} // namespace sql
} // namespace odb

#endif
