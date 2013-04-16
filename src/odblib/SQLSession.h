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

#include "eckit/machine.h"

#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"

namespace odb {
namespace sql {

class SQLOutput;
class SQLDatabase;
class SQLStatement;
class SQLTable;

class SQLSession {
public:
	SQLSession();
	virtual ~SQLSession(); // Change to virtual if base class


	SQLDatabase& openDatabase(const eckit::PathName&,const string& name = "");
	void closeDatabase(const string& name);

	void createIndex(const string&,const string&);

	SQLDatabase* getDatabase(const string& name);

	double getParameter(int) const;
	void   setParameter(int,double);

	SQLTable* findTable(const string&);
	SQLTable* findTable(const string&,const string&);
	SQLTable* findFile(const string&);

	SQLTable* openDataHandle(eckit::DataHandle &);
	SQLTable* openDataStream(istream &);

	virtual void statement(SQLStatement*) = 0;
	virtual SQLOutput* defaultOutput() = 0;
	unsigned long long lastExecuteResult() { return lastExecuteResult_; }

	SQLDatabase& currentDatabase() const;
	SQLDatabase& currentDatabase(SQLDatabase*);

	static SQLSession& current();

protected:

	unsigned long long lastExecuteResult_;

	unsigned long long execute(SQLStatement&);
	
	// void print(ostream&) const; // Change to virtual if base class	

private:
// No copy allowed

	SQLSession(const SQLSession&);
	SQLSession& operator=(const SQLSession&);

// -- Members

	SQLDatabase* current_;
	map<int,double> params_;

	map<string,SQLDatabase*> databases_;

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLSession& p)
	//	{ p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
