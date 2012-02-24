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
