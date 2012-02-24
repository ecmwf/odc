#ifndef Stack_H
#define Stack_H

// Headers
#include <machine.h>
#include <stack>

#include "SQLStatement.h"
#include "SQLColumn.h"
#include "SQLExpression.h"
#include "SQLOutput.h"
#include "FunctionROWNUMBER.h"
#include "SelectOneTable.h"

namespace odb {
namespace sql {

// Forward declarations

class Environment;

struct Stack : private stack<Environment*> {
	Stack();
	~Stack();

	void pushFrame(const SortedTables::iterator);
	void popFrame();

	void print(ostream& s) const;

	const SortedTables::iterator& tablesIterator(); 

	SelectOneTable& table();
	void table(SelectOneTable *);

	SQLTableIterator& cursor();
	void cursor(SQLTableIterator *);
};


} // namespace sql
} // namespace odb

#endif
