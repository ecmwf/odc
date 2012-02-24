#include "SQLExpressionEvaluated.h"
#include "SQLExpression.h"
#include "StrStream.h"
#include "SQLType.h"

namespace odb {
namespace sql {
namespace expression {

SQLExpressionEvaluated::SQLExpressionEvaluated(SQLExpression& e)
: type_(e.type()),
  missing_(false),
  value_(e.eval(missing_)),
  missingValue_(e.missingValue())
{}

SQLExpressionEvaluated::~SQLExpressionEvaluated() {}

void SQLExpressionEvaluated::print(ostream& o) const
{
	if (missing_)
		o << "NULL";
	else
		o << value_;
	o << ", ";
}

} // namespace expression
} // namespace sql
} // namespace odb

