/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/StrStream.h"

#include "odblib/SQLExpression.h"
#include "odblib/SQLExpressionEvaluated.h"
#include "odblib/SQLType.h"

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

