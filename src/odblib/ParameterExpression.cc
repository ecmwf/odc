/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/ParameterExpression.h"
#include "odblib/SQLAST.h"
#include "eclib/Log.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLSession.h"

namespace odb {
namespace sql {
namespace expression {

ParameterExpression::ParameterExpression(int which)
: value_(0),
  which_(which)
{
// don't use any Log::* here
//	cout << "new ParameterExpression " << name << endl;
}

ParameterExpression::ParameterExpression(const ParameterExpression& other)
: value_(other.value_),
  which_(other.which_)
{}


SQLExpression* ParameterExpression::ParameterExpression::clone() const { return new ParameterExpression(*this); }

ParameterExpression::~ParameterExpression() {}

double ParameterExpression::eval(bool& missing) const { return value_; }

void ParameterExpression::prepare(SQLSelect& sql)
{
	value_ = SQLSession::current().getParameter(which_);
//	cout << "ParameterExpression " << name_ << " " << value_ << endl;
}

void ParameterExpression::cleanup(SQLSelect& sql) { value_ = 0; }

void ParameterExpression::print(ostream& s) const
{
	s << '?' << which_ << '=' << value_;
}

bool ParameterExpression::isConstant() const { return false; }

} // namespace expression
} // namespace sql
} // namespace odb

