#include "OrderByExpressions.h"
#include "NumberExpression.h"
#include "StrStream.h"
#include "SQLType.h"
#include "SQLOutput.h"
#include "StringTool.h"

namespace odb {
namespace sql {
namespace expression {

bool OrderByExpressions::operator<(const OrderByExpressions& o) const
{
	size_t n = size();
	//ASSERT(n == o.size());

	for (size_t i = 0; i < n; ++i)
	{
		bool asc = ascending_[i];

		const SQLExpression& left = asc ? *(*this)[i] : *o[i];
		const SQLExpression& right = asc ? *o[i] : *(*this)[i];

		bool leftMissing = false;
		bool rightMissing = false;

		double leftValue = left.eval(leftMissing);
		double rightValue = right.eval(rightMissing);

		//TODO: handle missing value
		if (leftValue == rightValue)
			continue;

		if (left.type()->getKind() == type::SQLType::stringType)
		{
			string lv = StringTool::double_as_string(leftValue);
			string rv = StringTool::double_as_string(rightValue);
			if (! (lv.compare(rv) < 0))
				return false;
		} else {
			if (! (leftValue < rightValue))
				return false;
		}

		return true;
	}
	return false;
}


} // namespace expression
} // namespace sql
} // namespace odb

