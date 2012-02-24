// File SelectOneTable.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SelectOneTable_H
#define SelectOneTable_H

// Headers
#include <machine.h>
#include <stack>

#include "SQLStatement.h"
#include "SQLColumn.h"
#include "SQLExpression.h"
#include "SQLOutput.h"
#include "FunctionROWNUMBER.h"

namespace odb {
namespace sql {

// Forward declarations
class SQLTableIterator;

struct SelectOneTable {
	SelectOneTable(const SQLTable* table = 0);
	~SelectOneTable();

	const SQLTable*               table_;
	vector<SQLColumn*>            fetch_;
	vector<pair<double,bool>*>    values_;

	Expressions check_;
	Expressions index_;

	// For links
	pair<double,bool>*   offset_;
	pair<double,bool>*   length_;
	SQLColumn*           column_; // Reference column

	// For checking/debugging
	const SQLTable*    table1_;
	const SQLTable*    table2_;

	// For index

	// For sorting
	int order_;

};

typedef vector<SelectOneTable*> SortedTables;

} // namespace sql
} // namespace odb

#endif
