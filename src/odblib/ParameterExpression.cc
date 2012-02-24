#include "ParameterExpression.h"
#include "SQLAST.h"
#include "Log.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "SQLSession.h"

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

