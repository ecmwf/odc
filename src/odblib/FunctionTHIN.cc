/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionTHIN.h
/// (C) ECMWF July 2010

#include <string>
#include <utility>
#include <vector>

#include "eclib/Log.h"

#include "odblib/FunctionTHIN.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLSession.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionTHIN::FunctionTHIN(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args),
  count_(0)
{}

FunctionTHIN::FunctionTHIN(const FunctionTHIN& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_)
{}

FunctionTHIN::~FunctionTHIN() {}

SQLExpression* FunctionTHIN::clone() const { return new FunctionTHIN(*this); }

const odb::sql::type::SQLType* FunctionTHIN::type() const { return &odb::sql::type::SQLType::lookup("integer"); }

void FunctionTHIN::output(ostream& s) const
{
    bool missing;
    s << static_cast<unsigned long>(eval(missing));
}

void FunctionTHIN::print(ostream& s) const
{
    s << "THIN()";
}

double FunctionTHIN::eval(bool& missing) const
{
    int every_nth = (int) args_[0]->eval(missing);
    if ((*count_ - 1) % every_nth == 0)
      return 1.0;
    else
      return 0.0;
}

void FunctionTHIN::prepare(SQLSelect& sql)
{
    FunctionExpression::prepare(sql);
     count_ = &sql.count_;
}

void FunctionTHIN::cleanup(SQLSelect& sql)
{
}

bool FunctionTHIN::isConstant() const
{
    return false;
}

SQLExpression* FunctionTHIN::simplify(bool&)
{
    return 0;
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
