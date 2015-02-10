/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/FunctionNVL.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNVL::FunctionNVL(const std::string& name,const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionNVL::FunctionNVL(const FunctionNVL& other)
: FunctionExpression(other.name_, other.args_)
{}

FunctionNVL::~FunctionNVL() {}

SQLExpression* FunctionNVL::clone() const { return new FunctionNVL(*this); }

const type::SQLType* FunctionNVL::type() const { return &type::SQLType::lookup("real"); }

// Don't set the missing flag
double FunctionNVL::eval(bool& ) const
{
	bool missing = false;
	double x = args_[0]->eval(missing);
	return missing ? args_[1]->eval(missing) : x;
}


} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
