/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/StrStream.h"

#include "odblib/SQLExpression.h"
#include "odblib/NumberExpression.h"
#include "odblib/SQLType.h"
#include "odblib/SQLOutput.h"

namespace odb {
namespace sql {
namespace expression {

SQLExpression::SQLExpression() : isBitfield_(false), hasMissingValue_(false) {}

SQLExpression::~SQLExpression() {}

bool SQLExpression::isVector() const { return false; }

Vector& SQLExpression::vector()
{
	NOTIMP;
	return *((Vector*) 0);
}

Dictionary& SQLExpression::dictionary()
{
	NOTIMP;
	return *((Dictionary*) 0);
}

SQLExpression* SQLExpression::number(double value) { return new NumberExpression(value); } 

SQLExpression* SQLExpression::simplify(bool& changed)
{
	if(isConstant() && !isNumber())
	{
		changed = true;
		bool missing = false;
		Log::info() << "SIMPLIFY " << *this << " to " << eval(missing) << endl;
		return new NumberExpression(eval(missing));
	}
	return 0;
}

void SQLExpression::output(SQLOutput& s) const 
{ 
	bool missing = false;
	double d = eval(missing); 
	s.outputReal(d, missing);
}

void SQLExpression::title(const string& t)
{
	title_ = t;
}

string SQLExpression::title() const
{
	if(title_.size())
		return title_;

	StrStream s;
	s << *this << StrStream::ends;
	return string(s);
}


const type::SQLType* SQLExpression::type() const
{
	const type::SQLType& x = type::SQLType::lookup("real");
	return &x;
}

void SQLExpression::expandStars(const std::vector<SQLTable*>&, expression::Expressions& e)
{
	e.push_back(this);
}

} // namespace expression
} // namespace sql
} // namespace odb

