/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/log/Log.h"

//#include "odb_api/odblib/SQLAST.h"
//#include "odb_api/odblib/SQLBitfield.h"
//#include "odb_api/odblib/SchemaAnalyzer.h"
#include "odb_api/odblib/VariableExpression.h"
#include "odb_api/odblib/SQLDatabase.h"
#include "odb_api/odblib/SQLSession.h"

namespace odb {
namespace sql {
namespace expression {

const odb::sql::type::SQLType* VariableExpression::type() const { return &odb::sql::type::SQLType::lookup("real"); }

VariableExpression::VariableExpression(const std::string& name)
: value_(0),
  name_(name)
{}

VariableExpression::VariableExpression(const VariableExpression& other)
: value_(other.value_),
  name_(other.name_)
{}

SQLExpression* VariableExpression::clone() const { return new VariableExpression(*this); }

VariableExpression::~VariableExpression() {}

double VariableExpression::eval(bool& missing) const { return value_->eval(missing); }

void VariableExpression::prepare(SQLSelect& sql)
{
	SQLSession& s = SQLSession::current();
	value_ = s.currentDatabase().getVariable(name_);
//	std::cout << "VariableExpression " << name_ << " " << value_ << std::endl;
}

void VariableExpression::cleanup(SQLSelect& sql) { value_ = 0; }

void VariableExpression::print(std::ostream& s) const
{
	s << name_ << " => " << *value_;
}

bool VariableExpression::isConstant() const { return value_->isConstant(); }

bool VariableExpression::isVector() const { return value_->isVector(); }

} // namespace expression
} // namespace sql
} // namespace odb
