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
