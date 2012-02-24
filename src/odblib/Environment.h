#ifndef Environment_H
#define Environment_H

// Headers
#include <machine.h>
#include <stack>

#include "SQLStatement.h"
#include "SQLColumn.h"
#include "SQLExpression.h"
#include "SQLOutput.h"
#include "FunctionROWNUMBER.h"
#include "SelectOneTable.h"
#include "Stack.h"

namespace odb {
namespace sql {

// Forward declarations

class SQLTableIterator;

struct Environment {
	Environment (const SortedTables::iterator);
	~Environment ();

	SelectOneTable*& table();
	SQLTableIterator*& cursor();

	void print(ostream& s) const;

	const SortedTables::iterator tablesIterator_;
private:
	SelectOneTable* table_;
	SQLTableIterator* cursor_;
};

} // namespace sql
} // namespace odb

#endif
