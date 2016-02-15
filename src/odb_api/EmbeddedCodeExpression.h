/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File EmbeddedCodeExpression.h
// Piotr Kuchta - ECMWF Septembet 2015

#ifndef EmbeddedCodeExpression_H
#define EmbeddedCodeExpression_H

#include "odb_api/SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class EmbeddedCodeExpression : public SQLExpression {
public:
	EmbeddedCodeExpression(const std::string&);
	EmbeddedCodeExpression(const EmbeddedCodeExpression&);
	~EmbeddedCodeExpression(); 

	SQLExpression* clone() const;

private:
// No copy allowed
	EmbeddedCodeExpression& operator=(const EmbeddedCodeExpression&);

	std::string text_;

// -- Overridden methods
	virtual void print(std::ostream& s) const;
	virtual void expandStars(const std::vector<SQLTable*>&, expression::Expressions&);
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	const type::SQLType* type() const;
	virtual double eval(bool& missing) const;
	virtual bool isConstant() const { return false; }
	virtual bool isNumber() const { return false; }
	virtual void output(std::ostream& s) const;
};

} // namespace expression
} // namespace sql
} // namespace odb 

#endif
