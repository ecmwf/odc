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

#include "odb_api/Expressions.h"

namespace odb {
namespace sql {

class SQLColumn;

// Forward declarations
class SQLTableIterator;

struct SelectOneTable {
	SelectOneTable(const SQLTable* table = 0);
	~SelectOneTable();

	const SQLTable*               table_;
	std::vector<SQLColumn*>            fetch_;
    std::vector<std::pair<double*,bool&>>    values_;

	Expressions check_;
	Expressions index_;

	// For links
    std::pair<double*,bool&>   offset_;
    std::pair<double*,bool&>   length_;
	SQLColumn*           column_; // Reference column

	// For checking/debugging
	const SQLTable*    table1_;
	const SQLTable*    table2_;

	// For index

	// For sorting
	int order_;

};

typedef std::vector<SelectOneTable*> SortedTables;

} // namespace sql
} // namespace odb

#endif
