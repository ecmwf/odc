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

#include "odblib/FunctionNORM.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNORM::FunctionNORM(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  value_(0),
  resultNULL_(true)
{}

FunctionNORM::FunctionNORM(const FunctionNORM& other)
: FunctionExpression(other.name_, other.args_),
  value_(other.value_),
  resultNULL_(other.resultNULL_)
{}

SQLExpression* FunctionNORM::clone() const { return new FunctionNORM(*this); }

FunctionNORM::~FunctionNORM() {}

double FunctionNORM::eval(bool& missing) const
{
	if (resultNULL_)
	{
		missing = true;
		return (double) 0;
	}
    double lvalue =  (value_ > 0) ? sqrt(value_) : (double)0;
	return lvalue;
}

void FunctionNORM::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
}

void FunctionNORM::cleanup(SQLSelect& sql)
{
	FunctionExpression::cleanup(sql);
	value_ = 0;
}

void FunctionNORM::partialResult() 
{
	bool missing = false;
	double x = args_[0]->eval(missing);
	double y = args_[1]->eval(missing);
	if(! missing)
	{
		value_ += x*y;
		resultNULL_ = false;
	}
}

static FunctionMaker<FunctionNORM> make_NORM("norm",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

