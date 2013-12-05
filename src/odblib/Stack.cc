/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/log/BigNum.h"
//#include "eckit/exception/Exceptions.h"

//#include "odblib/SQLAST.h"
//#include "odblib/SQLColumn.h"
//#include "odblib/SQLDatabase.h"
//#include "odblib/SQLSelect.h"
//#include "odblib/SQLSession.h"
//#include "odblib/SQLTable.h"
//#include "odblib/SQLTable.h"
//#include "odblib/SQLType.h"
//#include "odblib/SchemaAnalyzer.h"

namespace odb {
namespace sql {

Stack::Stack() {}

Stack::~Stack() { while(size()) popFrame(); }

const SortedTables::iterator& Stack::tablesIterator() 
{
	//if (size() == 0) 
	//	return end();
	return top()->tablesIterator_;
}

SelectOneTable& Stack::table() { return *(top()->table()); }

void Stack::table(SelectOneTable *p) { top()->table() = p; ASSERT(top()->table()); }

SQLTableIterator& Stack::cursor() { return *(top()->cursor()); }

void Stack::cursor(SQLTableIterator *p) { top()->cursor() = p; }

void Stack::pushFrame(const SortedTables::iterator ti) 
{
	push(new Environment(ti));
}

void Stack::popFrame()
{
	if (size())
	{
		delete top();
		pop();
	}
}

} // namespace sql 
} // namespace odb 
