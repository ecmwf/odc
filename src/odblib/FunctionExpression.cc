/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <math.h>
#include <limits.h>

#include "eckit/exception/Exceptions.h"

#include "odblib/FunctionExpression.h"
#include "odblib/SQLSelect.h"
#include "odblib/piconst.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionExpression::FunctionExpression(const string& name, const expression::Expressions& args)
: name_(name),
  args_(args)
{
//  never use any logging here (Log::*)
//	std::cout << "new FunctionExpression " << name << std::endl;
}

FunctionExpression::FunctionExpression(const FunctionExpression& other)
: name_(other.name_),
  args_(other.args_)
{}


const type::SQLType* FunctionExpression::type() const { return &type::SQLType::lookup("double"); }

FunctionExpression::~FunctionExpression() {}

void FunctionExpression::prepare(SQLSelect& sql)
{
	for(expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j)
		(*j)->prepare(sql);
}

void FunctionExpression::cleanup(SQLSelect& sql)
{
	for(expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j)
		(*j)->cleanup(sql);
}

void FunctionExpression::partialResult()
{
	for(expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j)
		(*j)->partialResult();
}


SQLExpression* FunctionExpression::simplify(bool& changed)
{ 
	for(expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j)
	{
		SQLExpression* x = *j;
		SQLExpression* y = x->simplify(changed);

		if(y)
		{
			delete x;
			*j = y;
			std::cout << "SIMPLIFY " << *this << std::endl;
			changed = true;
		}
	}
	
	return SQLExpression::simplify(changed);
}


bool FunctionExpression::isConstant() const
{ 
	for(expression::Expressions::const_iterator j = args_.begin(); j != args_.end(); ++j)
		if(!(*j)->isConstant())
			return false;
	return true;
}

bool FunctionExpression::isAggregate() const
{ 
	for(expression::Expressions::const_iterator j = args_.begin(); j != args_.end(); ++j)
		if((*j)->isAggregate())
			return true;
	return false;
}

void FunctionExpression::print(std::ostream& s) const 
{
	s << name_;
	s << '(';
	for(expression::Expressions::const_iterator j = args_.begin(); j != args_.end(); ++j)
	{
		if(j != args_.begin()) s << ',';
		s << *(*j);
	}
	s << ')';
		
}

void FunctionExpression::tables(std::set<SQLTable*>& t)
{
	for(expression::Expressions::iterator j = args_.begin(); j != args_.end(); ++j)
		(*j)->tables(t);
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

