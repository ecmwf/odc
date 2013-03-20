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
#include "odblib/ColumnExpression.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLType.h"
#include "odblib/SQLTable.h"

using namespace eckit;

namespace odb {
namespace sql {
namespace expression {

static pair<double,bool> zero_(0,false);

ColumnExpression::ColumnExpression(const string& name, SQLTable* table, int begin, int end)
: type_(0),
  value_(&zero_),
  columnName_(name),
  table_(table),
  tableReference_(),
  beginIndex_(begin),
  endIndex_(end),
  nominalShift_(0)
{}

ColumnExpression::ColumnExpression(const string& name, const string& tableReference, int begin, int end)
: type_(0),
  value_(&zero_),
  columnName_(name),
  table_(0),
  tableReference_(tableReference),
  beginIndex_(begin),
  endIndex_(end),
  nominalShift_(0)
{}

ColumnExpression::ColumnExpression(const ColumnExpression& e)
: type_(e.type_),
  value_(e.value_),
  columnName_(e.columnName_),
  table_(e.table_),
  tableReference_(e.tableReference_),
  beginIndex_(e.beginIndex_),
  endIndex_(e.endIndex_),
  nominalShift_(e.nominalShift_)
{}

SQLExpression* ColumnExpression::clone() const { return new ColumnExpression(*this); }

ColumnExpression::~ColumnExpression() {}

double ColumnExpression::eval(bool& missing) const
{
	if(value_->second)
		missing = true;
	return value_->first;
}

void ColumnExpression::prepare(SQLSelect& sql)
{
	string fullName;

	SQLTable* table = sql.findTable(columnName_, &fullName, &hasMissingValue_, &missingValue_, &isBitfield_, &bitfieldDef_);

	if(!table_) table_ = table;
 
	value_ = sql.column(columnName_, table_);
	type_  = sql.typeOf(columnName_, table_);

	Log::debug() << "ColumnExpression::prepare: columnName_=" << columnName_ 
	<< ", title=" << title()
	<< ", table=" << table_->name()
	<< ", fullName =" << fullName
	<< " type=" << *type_ 
	<< " bitfieldDef.first.size =" << bitfieldDef_.first.size()
	<< endl;

	if (columnName_ == title() && columnName_ != fullName)
	{
		title(fullName);

		Log::debug() << "ColumnExpression::prepare: columnName_=" << columnName_ 
		<< ", title[PATCHED]=" << title()
		<< endl;
	}
}

void ColumnExpression::cleanup(SQLSelect& sql)
{
	value_ = &zero_;
	type_  = 0;
}


void ColumnExpression::print(ostream& s) const 
{
	s << columnName_;
	if (nominalShift_ != 0)
		s << "#" << nominalShift_;
	//if(table_) s << "@" << table_->fullName();
}

void ColumnExpression::output(SQLOutput& o) const 
{ 
	bool missing = false;
	double v = eval(missing);
	type_->output(o, v, missing); 
}

void ColumnExpression::expandStars(const std::vector<SQLTable*>& tables, expression::Expressions& e)
{
	ostream& L(Log::debug());
	L << "ColumnExpression::expandStars: expanding '" << columnName_ << "' (" << tableReference_ << ")" << endl;

	if(beginIndex_ != -1 && endIndex_ != -1)
	{
		ASSERT(beginIndex_ <= endIndex_);
		for(int i = beginIndex_; i <= endIndex_; i++)
			e.push_back(new ColumnExpression(columnName_ + "_" + Translator<int,string>()(i), this->table()));
		return;
	}

	if(columnName_ != "*")
	{
		e.push_back(this);
		return;
	}

	stringstream ss;
	
	unsigned int matched = 0;
	for(std::vector<SQLTable*>::const_iterator j = tables.begin();  j != tables.end(); ++j)
	{
		SQLTable* table = (*j);
		std::vector<string> names = table->columnNames();

		for(size_t i = 0; i < names.size(); i++)
		{
			if ((tableReference_.size())
				&& ((names[i].rfind(tableReference_) == string::npos)
					|| (names[i].rfind(tableReference_) + tableReference_.size() < names[i].size())))
			{
				L << "ColumnExpression::expandStars: skip '" << names[i] << "'" << endl;
				continue;
			}
			
			ss << names[i] << ", ";
			++matched;
			e.push_back(new ColumnExpression(names[i], table));
		}
	}
	if (! matched)
		throw eckit::UserError(string("No columns matching ") + columnName_ + tableReference_ + " found.");

	L << "ColumnExpression::expandStars: added " << ss.str() << endl;
	delete this;
}

bool ColumnExpression::indexed() 
{ 
	ASSERT(table_);
	SQLColumn* col = table_->column(columnName_);
	return col->hasIndex();
}

SQLIndex* ColumnExpression::getIndex(double* value)
{
	ASSERT(table_);
	SQLColumn* col = table_->column(columnName_);
	return col->getIndex(value);
}

void ColumnExpression::tables(set<SQLTable*>& t) 
{ 
	ASSERT(table_);
	t.insert(table_); 
}

} // namespace expression
} // namespace sql
} // namespace odb

