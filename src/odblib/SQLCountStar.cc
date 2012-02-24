#include "Log.h"
#include "SQLCountStar.h"
#include "SQLAST.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "SQLDatabase.h"
#include "SQLTable.h"

namespace odb {
namespace sql {

SQLCountStar::SQLCountStar(const string& table):
	table_(table)
{} 

SQLCountStar::~SQLCountStar() {}

void SQLCountStar::print(ostream& s) const
{
	s << "SELECT COUNT(*) FROM " << table_;
}

unsigned long long SQLCountStar::execute(SQLDatabase& db)
{
	unsigned long long n = db.table(table_)->noRows();	
	cout << n << endl;	
	return n;
}

} // namespace sql
} // namespace odb
