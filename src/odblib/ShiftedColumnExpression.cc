/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eclib/Log.h"
#include "eclib/Translator.h"

#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/ShiftedColumnExpression.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLType.h"
#include "odblib/SQLTable.h"

namespace odb {
namespace sql {
namespace expression {

static pair<double,bool> zero_(0,false);

// FIXME: we need to retrieve actual value of missing value for this column
double const MISSING_VALUE_REAL = -2147483647.0;
static pair<double,bool> missing_(MISSING_VALUE_REAL,true);

ShiftedColumnExpression::ShiftedColumnExpression(const string& name, SQLTable* table, int shift, int begin, int end)
: ColumnExpression(name, table, begin, end),
  shift_(shift),
  oldValues_()
{
	for (size_t i = 0; i < shift_; ++i)
		oldValues_.push_front(missing_);
}

ShiftedColumnExpression::ShiftedColumnExpression(const string& name, const string& tableReference, int shift, int begin, int end)
: ColumnExpression(name, tableReference, begin, end),
  shift_(shift),
  oldValues_()
{
	for (size_t i = 0; i < shift_; ++i)
		oldValues_.push_front(missing_);
}

ShiftedColumnExpression::ShiftedColumnExpression(const ShiftedColumnExpression& e)
: ColumnExpression(e),
  shift_(e.shift_),
  oldValues_(e.oldValues_)
{}

SQLExpression* ShiftedColumnExpression::clone() const { return new ShiftedColumnExpression(*this); }
ShiftedColumnExpression::~ShiftedColumnExpression() {}
void ShiftedColumnExpression::print(ostream& s) const { s << columnName_ << "#" << shift_; }


double ShiftedColumnExpression::eval(bool& missing) const
{

	pair<double,bool> const& v(oldValues_.back());
	double value = v.first;
	bool miss = v.second;

	list<pair<double,bool> >&oldValues(*const_cast<list<pair<double,bool> >*>(&oldValues_));
	oldValues.pop_back();
	oldValues.push_front(*value_);

	if(miss) missing = true;
	return value;
}

void ShiftedColumnExpression::cleanup(SQLSelect& sql)
{
	value_ = &zero_;
	type_  = 0;
	oldValues_.clear();
}


void ShiftedColumnExpression::output(SQLOutput& o) const 
{ 
	//Log::info() << "ShiftedColumnExpression::output:" << endl;

	bool missing = false;
	double v = eval(missing);
	type_->output(o, v, missing); 
}

} // namespace expression
} // namespace sql
} // namespace odb

