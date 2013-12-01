/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/BitColumnExpression.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLBit.h"
#include "odblib/SQLTable.h"

using namespace eckit;

namespace odb {
namespace sql {
namespace expression {

BitColumnExpression::BitColumnExpression(const std::string& name, const std::string& field, SQLTable* table)
: ColumnExpression(name + "." + field + "@" + table->name(), table),
  mask_(0),
  bitShift_(0),
  field_(field),
  name_(name)
{
	Log::info() << "BitColumnExpression::BitColumnExpression: name=" << name
		<< ", field=" << field << ", table->name() =" << table->name()
		<< ": name_=" << name_
		<< std::endl;
}

BitColumnExpression::BitColumnExpression(const std::string& name, const std::string& field, const std::string& tableReference)
: ColumnExpression(name + "." + field + tableReference, tableReference),
  mask_(0),
  bitShift_(0),
  field_(field),
  name_(name)
{
	Log::info() << "BitColumnExpression::BitColumnExpression: name=" << name
		<< ", field=" << field << ", tableReference=" << tableReference
		<< ": name_=" << name_
		<< std::endl;
}

BitColumnExpression::BitColumnExpression(const BitColumnExpression& o)
: ColumnExpression(o),
  mask_(o.mask_),
  bitShift_(o.bitShift_),
  field_(o.field_),
  name_(o.name_)
{}

BitColumnExpression::~BitColumnExpression() {}

const odb::sql::type::SQLType* BitColumnExpression::type() const
{
// Change the type to integer to be able to create a new ODA if necessary
	return &odb::sql::type::SQLType::lookup("integer");
}

void BitColumnExpression::prepare(SQLSelect& sql)
{
	std::string name = name_ + "." + field_ + tableReference_;
	if(!table_)
		table_ = sql.findTable(name);
	value_ = sql.column(name, table_);
	type_  = sql.typeOf(name, table_);


	const type::SQLBit* bit = dynamic_cast<const type::SQLBit*>(type_);
	if(bit)
	{
		mask_  = bit->mask();
		bitShift_ = bit->shift();
	}
	else
	{
		// This is for .length and .offset
		// Not very nice, I know
		mask_  = 0xffffffff;
		bitShift_ = 0;
	}

}

double BitColumnExpression::eval(bool& missing) const
{
	if(value_->second) missing = true;
	unsigned long x = static_cast<unsigned long>(value_->first);
	return (x & mask_) >> bitShift_;
}

void BitColumnExpression::expandStars(const std::vector<SQLTable*>& tables, expression::Expressions& e)
{
	// TODO: regex
	if(field_ != "*")
	{
		e.push_back(this);
		return;
	}

    for(std::vector<SQLTable*>::const_iterator j = tables.begin();  j != tables.end(); ++j)
	{

		SQLTable* table = (*j);
        std::vector<std::string> names = table->bitColumnNames(name_ + tableReference_);

		for(size_t i = 0; i < names.size(); i++)
		{
			e.push_back(new BitColumnExpression(name_, names[i], tableReference_ /*table*/));
		}
	}

	delete this;
}

} // namespace expression
} // namespace sql
} // namespace odb

