/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLSession.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef odb_sql_SQLSession_H
#define odb_sql_SQLSession_H

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }
namespace eckit { class ExecutionContext; }

#include "eckit/eckit.h"

#include "odb_api/SQLSelectFactory.h"

namespace odb {
namespace sql {

class SQLOutput;
class SQLDatabase;
class SQLStatement;
class SQLTable;

class SQLSession {
public:
	SQLSession();
	virtual ~SQLSession(); 

	SQLDatabase& openDatabase(const eckit::PathName&,const std::string& name = "");
	void closeDatabase(const std::string& name);

	void createIndex(const std::string&,const std::string&);

	SQLDatabase* getDatabase(const std::string& name);

    SQLSelectFactory& selectFactory();

	double getParameter(int) const;
	void   setParameter(int,double);

	SQLTable* findTable(const std::string&);
	SQLTable* findTable(const std::string&,const std::string&);
	SQLTable* findFile(const std::string&);

	SQLTable* openDataHandle(eckit::DataHandle &);
    SQLTable* openDataStream(std::istream &, const std::string &);

	virtual void statement(SQLStatement*) = 0;
	virtual SQLOutput* defaultOutput() = 0;
	unsigned long long lastExecuteResult() { return lastExecuteResult_; }

	SQLDatabase& currentDatabase() const;
	SQLDatabase& currentDatabase(SQLDatabase*);

	unsigned long long execute(SQLStatement&, eckit::ExecutionContext*);

protected:
	unsigned long long lastExecuteResult_;

private:
// No copy allowed

	SQLSession(const SQLSession&);
	SQLSession& operator=(const SQLSession&);

	SQLDatabase* currentDatabase_;
    std::map<int,double> params_;
    std::map<std::string,SQLDatabase*> databases_;
    SQLSelectFactory selectFactory_;
};

} // namespace sql
} // namespace odb

#endif
