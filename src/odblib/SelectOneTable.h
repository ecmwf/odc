/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SelectOneTable.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SelectOneTable_H
#define SelectOneTable_H

#include <stack>

#include "eckit/eckit.h"

#include "odblib/SQLStatement.h"
#include "odblib/SQLColumn.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLOutput.h"
#include "odblib/FunctionROWNUMBER.h"

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
	std::pair<double,bool>*   offset_;
	std::pair<double,bool>*   length_;
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
