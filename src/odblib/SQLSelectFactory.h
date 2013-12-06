/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SQLSelectFactory_H
#define SQLSelectFactory_H

//#include "eckit/io/DataHandle.h"
//#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/ThreadSingleton.h"

//#include "odblib/SQLExpression.h"
#include "odblib/SQLOutputConfig.h"
//#include "odblib/SQLSelect.h"
//#include "odblib/SQLTable.h"
#include "odblib/Expressions.h"

namespace odb {

    class DataTable;
    class SQLDatabase;

namespace sql {

class SQLSession;

class SQLSelectFactory {
public:

	static SQLSelectFactory& instance();

	SQLSelect* create(bool distinct,
		Expressions select_list,
		std::string into,
		std::vector<SQLTable*> from,
		odb::sql::expression::SQLExpression *where,
		Expressions group_by,
		std::pair<Expressions,std::vector<bool> > order_by);

	SQLExpression* createColumn(
		const std::string& columnName,
		const std::string& bitfieldName,
		const SQLExpression* vectorIndex,
		const std::string& table,
		const SQLExpression* pshift);

    eckit::DataHandle* implicitFromTableSource() { return implicitFromTableSource_; }
    void implicitFromTableSource(eckit::DataHandle* h) { implicitFromTableSource_ = h; }

    istream* implicitFromTableSourceStream() { return implicitFromTableSourceStream_; }
    void implicitFromTableSourceStream(std::istream* is) { implicitFromTableSourceStream_ = is; }

	SQLDatabase* database() { return database_; }
	void database(SQLDatabase* db) { database_ = db; }

	SQLOutputConfig config() { return config_; }
	void config(SQLOutputConfig cfg) { config_ = cfg; }

	std::string csvDelimiter() { return csvDelimiter_; }
	void csvDelimiter(const std::string& d) { csvDelimiter_ = d; }

private:

    SQLSelectFactory();

     // No copy allowed
     SQLSelectFactory(const SQLSelectFactory&);
     SQLSelectFactory& operator=(const SQLSelectFactory&);

	std::string index(const std::string& columnName, const SQLExpression* index);

	void reshift(Expressions&);

	SQLExpression* reshift(SQLExpression*);

    void resolveImplicitFrom(SQLSession&, std::vector<SQLTable*>& from);

    eckit::DataHandle* implicitFromTableSource_;

    istream* implicitFromTableSourceStream_;

	SQLDatabase* database_;
	SQLOutputConfig config_;
	int maxColumnShift_;
	int minColumnShift_;
	std::string csvDelimiter_;

    friend class eckit::NewAlloc0<SQLSelectFactory>;
};

} // namespace sql
} // namespace odb

#endif
