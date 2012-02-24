#include "NumberExpression.h"

namespace odb {
namespace sql {
namespace expression {

NumberExpression::NumberExpression(double value)
: value_(value)
{}

NumberExpression::NumberExpression(const NumberExpression& other)
: value_(other.value_)
{}

SQLExpression* NumberExpression::clone() const { return new NumberExpression(*this); }

NumberExpression::~NumberExpression() {}

double NumberExpression::eval(bool& missing) const { return value_; }

void NumberExpression::prepare(SQLSelect& sql) {}

void NumberExpression::cleanup(SQLSelect& sql) {}

void NumberExpression::print(ostream& s) const { s << value_; }

} // namespace expression
} // namespace sql
} // namespace odb
