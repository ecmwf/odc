#include "Exceptions.h"
#include "Log.h"
#include "SQLAST.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "SQLInteractiveSession.h"
#include "SQLSimpleOutput.h"
#include "SQLDatabase.h"
#include "SQLStatement.h"

namespace odb {
namespace sql {

SQLInteractiveSession::SQLInteractiveSession(ostream &out)
: out_(out)
{}

SQLInteractiveSession::~SQLInteractiveSession()
{}

SQLOutput* SQLInteractiveSession::defaultOutput()
{
	return new SQLSimpleOutput(out_);
}

void SQLInteractiveSession::statement(SQLStatement *sql)
{
	ASSERT(sql);	
	execute(*sql);
	delete sql;
}

} // namespace sql
} // namespace odb
