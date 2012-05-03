/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/OrderByExpressions.h"
#include "odblib/NumberExpression.h"
#include "eclib/StrStream.h"
#include "odblib/SQLType.h"
#include "odblib/SQLOutput.h"
#include "odblib/StringTool.h"

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

