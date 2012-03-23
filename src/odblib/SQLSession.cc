/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Exceptions.h"
#include "eclib/Log.h"
#include "eclib/ThreadSingleton.h"
#include "eclib/Timer.h"

#include "odblib/ODADatabase.h"
#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLColumn.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLSession.h"
#include "odblib/SQLStatement.h"
#include "odblib/SQLTable.h"
#include "odblib/SchemaAnalyzer.h"

template class ThreadSingleton<odb::sql::SQLSession*>;
static ThreadSingleton<odb::sql::SQLSession*> instance_;

namespace odb {
namespace sql {

string defaultDB = "default";

SQLSession::SQLSession():
	current_(0)
{
	//ASSERT(!instance_.instance());
	instance_.instance() = this;

	current_ = new ODADatabase(".", defaultDB); 
	current_->open();
	databases_[defaultDB] = current_;
}

SQLDatabase& SQLSession::currentDatabase(SQLDatabase *db)
{
	delete current_;
	current_ = db;
	current_->open();
	databases_[defaultDB] = current_;
	return *current_;
}

SQLSession::~SQLSession()
{
	//instance_.instance() = (SQLSession*)0;
	//cerr << "SQLSession::~SQLSession" << endl;
	for(map<string,SQLDatabase*>::iterator j = databases_.begin(); j != databases_.end(); ++j)
		delete (*j).second;
}

SQLSession& SQLSession::current()
{
	//ASSERT(instance_.instance());
	SQLSession* x = instance_.instance();
	return *x;
}

SQLDatabase& SQLSession::openDatabase(const PathName& path,const string& name)
{
	map<string,SQLDatabase*>::iterator j = databases_.find(name);
	if(j != databases_.end())
	{
		SQLDatabase* db = (*j).second;
		db->close();
		delete db;
	}

	current_ = new ODADatabase(path,name);
	current_->open();

	databases_[name] = current_;
	return *current_;
}

unsigned long long SQLSession::execute(SQLStatement& sql)
{
	Timer timer("Execute");
	ASSERT(current_);	

	currentSQLStatement_ = &sql;
	unsigned long long n = sql.execute();
	currentSQLStatement_ = 0;
	return lastExecuteResult_ = n;
}

SQLDatabase& SQLSession::currentDatabase() const
{
	ASSERT(current_);	
	return *current_;
}

SQLStatement& SQLSession::currentSQLStatement() const
{
	ASSERT(currentSQLStatement_);
	return *currentSQLStatement_;
}

double SQLSession::getParameter(int which) const
{
	map<int,double>::const_iterator j = params_.find(which);
	if(j == params_.end())
		throw UserError("Undefined parameter");
	return (*j).second;
}

void SQLSession::setParameter(int which,double value)
{
	params_[which] = value;
}

SQLDatabase* SQLSession::getDatabase(const string& name)
{
	map<string,SQLDatabase*>::iterator j = databases_.find(name);
	if(j == databases_.end())
		throw UserError("Cannot find database", name);
	return (*j).second;
}

SQLTable* SQLSession::findFile(const string& name)
{
	ASSERT(current_);
	return current_->table(name);
}

SQLTable* SQLSession::findTable(const string& name)
{
	ASSERT(current_);
	return current_->table(name);
}

SQLTable* SQLSession::openDataStream(istream &is)
{
	ASSERT(current_);
	return current_->openDataStream(is);
}

SQLTable* SQLSession::openDataHandle(DataHandle &dh)
{
	ASSERT(current_);
	return current_->openDataHandle(dh);
}

SQLTable* SQLSession::findTable(const string& database,const string& name)
{
	return getDatabase(database)->table(name);
}

void SQLSession::createIndex(const string& column,const string& table)
{
	ASSERT(current_);
#if 0
	current_->table(table)->column(column)->createIndex();
#endif
}

} // namespace sql
} // namespace odb
