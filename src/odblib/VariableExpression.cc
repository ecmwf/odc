#include "Log.h"
#include "SQLAST.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "VariableExpression.h"
#include "SQLDatabase.h"
#include "SQLSession.h"

namespace odb {
namespace sql {
namespace expression {

VariableExpression::VariableExpression(const string& name)
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
//	cout << "VariableExpression " << name_ << " " << value_ << endl;
}

void VariableExpression::cleanup(SQLSelect& sql) { value_ = 0; }

void VariableExpression::print(ostream& s) const
{
	s << name_ << " => " << *value_;
}

bool VariableExpression::isConstant() const { return value_->isConstant(); }

bool VariableExpression::isVector() const { return value_->isVector(); }

} // namespace expression
} // namespace sql
} // namespace odb
