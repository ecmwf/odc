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

Environment::Environment(const SortedTables::iterator ti)
: tablesIterator_(ti), table_(0), cursor_(0)
{}

SelectOneTable*& Environment::table() { return table_; }

SQLTableIterator*& Environment::cursor() { return cursor_; }

Environment::~Environment() { delete cursor_; }

} // namespace sql 
} // namespace odb 
