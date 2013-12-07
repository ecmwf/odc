/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/VariablesTable.h"
//#include "eckit/compat/StrStream.h"
//#include "eckit/parser/Tokenizer.h"

//#include "odblib/SQLAST.h"
//#include "odblib/SQLBitColumn.h"
//#include "odblib/SQLBitfield.h"
//#include "odblib/SQLColumn.h"
#include "odblib/SQLSession.h"
#include "odblib/SQLDatabase.h"
//#include "odblib/VariablesTable.h"
//#include "odblib/SQLType.h"
#include "odblib/SQLColumn.h"
//#include "odblib/SchemaAnalyzer.h"
#include "odblib/StringTool.h"
#include "odblib/SQLExpression.h"

using namespace eckit;

namespace odb {
namespace sql {

class VariablesColumn : public SQLColumn {
public:
	VariablesColumn(const std::string& name, int i, const std::string sqlType, SQLTable& owner)  
	: SQLColumn(type::SQLType::lookup(sqlType), owner, name, i, false, 0 /*FIXME*/)
	{
        missing_ = false;
        //value_ = MISSING_VALUE_REAL;
    }
     

    double next(bool& missing) 
    {
        NOTIMP;
    }

    double* value_;
    bool  missing_;
};

VariablesTable::VariablesTable(SQLDatabase& owner, const std::string& name)
: SQLTable(owner, "", name)
{
	addColumn(new VariablesColumn("name", 0, "std::string", *this), "name", 0);
	addColumn(new VariablesColumn("value", 1, "real", *this), "value", 1);
}

VariablesTable::~VariablesTable() {}

SQLTableIterator* VariablesTable::iterator(const std::vector<SQLColumn*>&) const
{
	return new VariablesTableIterator(SQLSession::current().currentDatabase().variables());
}

SQLColumn* VariablesTable::createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double missingValue)
{ NOTIMP; }

SQLColumn* VariablesTable::createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double missingValue, const BitfieldDef&)
{ NOTIMP; }

VariablesTableIterator::~VariablesTableIterator() {}

void VariablesTableIterator::rewind()
{
    it_ = variables_.begin();
}

bool VariablesTableIterator::next()
{
    // TODO: populate values
    if (it_ == variables_.end())
        return false;
    ++it_;
    data_[0] = StringTool::cast_as_double(it_->first);
    bool missing;
    data_[1] = it_->second->eval(missing);
    return true;
}

VariablesTableIterator::VariablesTableIterator(std::map<std::string,SQLExpression*>& vs)
: variables_(vs)
{
}


} // namespace sql 
} // namespace odb 

