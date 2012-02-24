#include "ConstantExpression.h"
#include "NumberExpression.h"
#include "StrStream.h"
#include "SQLType.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace expression {

ConstantExpression::ConstantExpression(double v, bool missing, const odb::sql::type::SQLType* type)
: value_(v), missing_(missing), type_(*type)
{}

ConstantExpression::~ConstantExpression() {}

void ConstantExpression::output(SQLOutput& o) const 
{ 
    type_.output(o, value_, missing_);
}

const type::SQLType* ConstantExpression::type() const { return &type_; }

} // namespace expression
} // namespace sql
} // namespace odb

