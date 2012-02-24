// File SQLSession.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLSession_H
#define SQLSession_H

class PathName;
class DataHandle;

#include "machine.h"
#include "SQLAST.h"
#include "SchemaAnalyzer.h"

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


	SQLDatabase& openDatabase(const PathName&,const string& name = "");
	void closeDatabase(const string& name);

	void createIndex(const string&,const string&);

	SQLDatabase* getDatabase(const string& name);

	double getParameter(int) const;
	void   setParameter(int,double);

	SQLTable* findTable(const string&);
	SQLTable* findTable(const string&,const string&);
	SQLTable* findFile(const string&);

	SQLTable* openDataHandle(DataHandle &);
	SQLTable* openDataStream(istream &);

	virtual void statement(SQLStatement*) = 0;
	virtual SQLOutput* defaultOutput() = 0;
	unsigned long long lastExecuteResult() { return lastExecuteResult_; }

	SQLDatabase& currentDatabase() const;
	SQLDatabase& currentDatabase(SQLDatabase*);

	SQLStatement& currentSQLStatement() const;

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

	SQLStatement* currentSQLStatement_;

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLSession& p)
	//	{ p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
