/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/FunctionCOUNT.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionCOUNT::FunctionCOUNT(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args),
  count_(0)
{}

FunctionCOUNT::FunctionCOUNT(const FunctionCOUNT& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_)
{}

SQLExpression* FunctionCOUNT::clone() const
{ return new FunctionCOUNT(*this); }

FunctionCOUNT::~FunctionCOUNT() {}

double FunctionCOUNT::eval(bool& missing) const
{
//cout << "FunctionCOUNT " << count_ << endl;
	return count_;
}

void FunctionCOUNT::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	count_ = 0;
}

void FunctionCOUNT::cleanup(SQLSelect& sql)
{
	FunctionExpression::cleanup(sql);
	count_ = 0;
}

void FunctionCOUNT::partialResult() 
{
	bool missing = false;
	args_[0]->eval(missing);
	if(!missing)
		count_++;
//cout << "FunctionCOUNT::partialResult " << count_ << endl;
}

static FunctionMaker<FunctionCOUNT> make_COUNT("count",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

