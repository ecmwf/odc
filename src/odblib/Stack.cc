/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "SQLSelect.h"
#include "SQLTable.h"
#include "Exceptions.h"
#include "SQLAST.h"
#include "SchemaAnalyzer.h"
#include "SQLDatabase.h"
#include "SQLColumn.h"
#include "SQLTable.h"
#include "SQLType.h"
#include "BigNum.h"
#include "SQLSession.h"

namespace odb {
namespace sql {

Stack::Stack() {}

Stack::~Stack() { while(size()) popFrame(); }

const SortedTables::iterator& Stack::tablesIterator() { return top()->tablesIterator_; }

SelectOneTable& Stack::table() { return *(top()->table()); }

void Stack::table(SelectOneTable *p) { top()->table() = p; ASSERT(top()->table()); }

SQLTableIterator& Stack::cursor() { return *(top()->cursor()); }

void Stack::cursor(SQLTableIterator *p) { top()->cursor() = p; }

void Stack::pushFrame(const SortedTables::iterator ti) {
	push(new Environment(ti));
}

void Stack::popFrame() {
	delete top();
	pop();
}

} // namespace sql 
} // namespace odb 
