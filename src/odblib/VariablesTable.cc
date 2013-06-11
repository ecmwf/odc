/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/compat/StrStream.h"
#include "eckit/utils/Tokenizer.h"

#include "odblib/SQLAST.h"
#include "odblib/SQLBitColumn.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLColumn.h"
#include "odblib/SQLSession.h"
#include "odblib/SQLDatabase.h"
#include "odblib/VariablesTable.h"
#include "odblib/SQLType.h"
#include "odblib/SQLColumn.h"
#include "odblib/SchemaAnalyzer.h"

using namespace eckit;

namespace odb {
namespace sql {

class VariablesColumn : public SQLColumn {
public:
	VariablesColumn(const string& name, int i, const string sqlType, SQLTable& owner)  
	: SQLColumn(type::SQLType::lookup(sqlType), owner, name, i, false, 0 /*FIXME*/, false, *((const BitfieldDef*) 0))
	{}
};

VariablesTable::VariablesTable(SQLDatabase& owner, const string& name)
: SQLTable(owner, "", name)
{
	addColumn(new VariablesColumn("name", 0, "STRING", *this), "name", 0);
	addColumn(new VariablesColumn("value", 1, "REAL", *this), "value", 1);
}

VariablesTable::~VariablesTable() {}

SQLTableIterator* VariablesTable::iterator(const vector<SQLColumn*>&) const
{
	return new VariablesTableIterator(SQLSession::current().currentDatabase().variables());
}

SQLColumn* VariablesTable::createSQLColumn(const type::SQLType& type, const string& name, int index, bool hasMissingValue, double missingValue, bool isBitfield, const BitfieldDef&)
{ 
	NOTIMP;
}

VariablesTableIterator::~VariablesTableIterator() {}

void VariablesTableIterator::rewind()
{
	NOTIMP;
}

bool VariablesTableIterator::next()
{
	NOTIMP;
}

VariablesTableIterator::VariablesTableIterator(map<string,SQLExpression*>& vs)
: variables_(vs)
{
}


} // namespace sql 
} // namespace odb 

