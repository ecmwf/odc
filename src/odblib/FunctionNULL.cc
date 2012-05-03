/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/FunctionNULL.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNULL::FunctionNULL(const string& name,const expression::Expressions& args)
: FunctionExpression(name, args)
{} 

FunctionNULL::FunctionNULL(const FunctionNULL& other)
: FunctionExpression(other.name_, other.args_)
{} 

FunctionNULL::~FunctionNULL() {}

SQLExpression* FunctionNULL::clone() const { return new FunctionNULL(*this); }

// Don't set the missing flag
double FunctionNULL::eval(bool& ) const
{
	bool missing = false;
	args_[0]->eval(missing);
	return missing;
}

static FunctionMaker<FunctionNULL> make_NULL("null",1);
static FunctionMaker<FunctionNULL> make_ISNULL("isnull",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

