/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SQLSelectFactory_H
#define SQLSelectFactory_H

#include "eclib/DataHandle.h"
#include "eclib/ThreadSingleton.h"

#include "odblib/SQLExpression.h"
#include "odblib/SQLOutputConfig.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLTable.h"

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
