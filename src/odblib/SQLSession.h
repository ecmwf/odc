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

#ifndef SQLSession_H
#define SQLSession_H

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

#include "eckit/eckit.h"


//#include "odblib/SQLAST.h"
//#include "odblib/SchemaAnalyzer.h"

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

	static SQLSession& current();

protected:

	unsigned long long lastExecuteResult_;

	unsigned long long execute(SQLStatement&);
	
	// void print(std::ostream&) const; 	

private:
// No copy allowed

	SQLSession(const SQLSession&);
	SQLSession& operator=(const SQLSession&);

// -- Members

	SQLDatabase* current_;
    std::map<int, double> params_;

    std::map<std::string, SQLDatabase*> databases_;

// -- Friends

	//friend std::ostream& operator<<(std::ostream& s,const SQLSession& p)
	//	{ p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
