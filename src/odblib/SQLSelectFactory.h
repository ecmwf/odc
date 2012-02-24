#ifndef SQLSelectFactory_H
#define SQLSelectFactory_H

#include "SQLExpression.h"
#include "SQLSelect.h"
#include "SQLTable.h"
#include "DataHandle.h"
#include "ThreadSingleton.h"
#include "SQLOutputConfig.h"

namespace odb { namespace sql { class SQLSelectFactory; } }

namespace odb {
namespace sql {

class SQLSelectFactory {
public:

	static SQLSelectFactory& instance();

	SQLSelect* create(bool distinct,
		Expressions select_list,
		string into,
		vector<SQLTable*> from,
		odb::sql::expression::SQLExpression *where,
		Expressions group_by,
		pair<Expressions,vector<bool> > order_by);

    DataHandle* implicitFromTableSource() { return implicitFromTableSource_; }
    void implicitFromTableSource(DataHandle* h) { implicitFromTableSource_ = h; }

    istream* implicitFromTableSourceStream() { return implicitFromTableSourceStream_; }
    void implicitFromTableSourceStream(istream* is) { implicitFromTableSourceStream_ = is; }

	SQLDatabase* database() { return database_; }
	void database(SQLDatabase* db) { database_ = db; }

	SQLOutputConfig config() { return config_; }
	void config(SQLOutputConfig cfg) { config_ = cfg; }

private:
	SQLSelectFactory();

// No copy allowed
	SQLSelectFactory(const SQLSelectFactory&);
	SQLSelectFactory& operator=(const SQLSelectFactory&);

    DataHandle* implicitFromTableSource_;
    istream* implicitFromTableSourceStream_;
	SQLDatabase* database_;
	SQLOutputConfig config_;

friend class ThreadSingleton<SQLSelectFactory>;
};

} // namespace sql
} // namespace odb

#endif
