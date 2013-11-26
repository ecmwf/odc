/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Environment_H
#define Environment_H

#include <stack>

#include "eckit/eckit.h"

#include "odblib/SQLStatement.h"
#include "odblib/SQLColumn.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLOutput.h"
#include "odblib/FunctionROWNUMBER.h"

#include "odblib/SelectOneTable.h"
#include "odblib/Stack.h"

namespace odb {
namespace sql {

// Forward declarations

class SQLTableIterator;

struct Environment {
	Environment (const SortedTables::iterator);
	~Environment ();

	SelectOneTable*& table();
	SQLTableIterator*& cursor();

	void print(std::ostream& s) const;

	const SortedTables::iterator tablesIterator_;
private:
	SelectOneTable* table_;
	SQLTableIterator* cursor_;
};

} // namespace sql
} // namespace odb

#endif
