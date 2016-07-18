/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Timer.h"
#include "odb_api/ODADatabase.h"
#include "odb_api/SQLDatabase.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLStatement.h"
#include "odb_api/SQLSelectFactory.h"

using namespace eckit;

namespace odb {
namespace sql {

std::string defaultDB = "default";

SQLSession::SQLSession()
: currentDatabase_(0)
{
    currentDatabase_ = new ODADatabase(".", defaultDB); 
    currentDatabase_->open();
    databases_[defaultDB] = currentDatabase_;
}

SQLSession::~SQLSession()
{
    for(std::map<std::string,SQLDatabase*>::iterator j = databases_.begin(); j != databases_.end(); ++j)
        delete (*j).second;
}

SQLSelectFactory& SQLSession::selectFactory() { return selectFactory_; }

SQLDatabase& SQLSession::currentDatabase(SQLDatabase *db)
{
    delete currentDatabase_;
    currentDatabase_ = db;
    currentDatabase_->open();
    databases_[defaultDB] = currentDatabase_;
    return *currentDatabase_;
}

SQLDatabase& SQLSession::openDatabase(const PathName& path,const std::string& name)
{
    std::map<std::string,SQLDatabase*>::iterator j = databases_.find(name);
    if(j != databases_.end())
    {
        SQLDatabase* db = (*j).second;
        db->close();
        delete db;
    }

    currentDatabase_ = new ODADatabase(path,name);
    currentDatabase_->open();

    databases_[name] = currentDatabase_;
    return *currentDatabase_;
}

unsigned long long SQLSession::execute(SQLStatement& sql, ecml::ExecutionContext* context)
{
    Timer timer("Execute");
    ASSERT(currentDatabase_);	

    unsigned long long n = sql.execute(context);
    return lastExecuteResult_ = n;
}

SQLDatabase& SQLSession::currentDatabase() const
{
    ASSERT(currentDatabase_);	
    return *currentDatabase_;
}

double SQLSession::getParameter(int which) const
{
    std::map<int,double>::const_iterator j = params_.find(which);
    if(j == params_.end())
        throw eckit::UserError("Undefined parameter");
    return (*j).second;
}

void SQLSession::setParameter(int which,double value)
{
    params_[which] = value;
}

SQLDatabase* SQLSession::getDatabase(const std::string& name)
{
    std::map<std::string,SQLDatabase*>::iterator j = databases_.find(name);
    if(j == databases_.end())
        throw eckit::UserError("Cannot find database", name);
    return (*j).second;
}

SQLTable* SQLSession::findTable(const odb::sql::Table& t)
{
    if (t.database.size())
        return getDatabase(t.database)->table(t);
    else
    {
        if (! currentDatabase_) 
            throw eckit::UserError("No current database", t.name);

        return currentDatabase_->table(t);
    }
}

SQLTable* SQLSession::openDataStream(std::istream &is, const std::string& delimiter)
{
    ASSERT(currentDatabase_);
    return currentDatabase_->openDataStream(is, delimiter);
}

SQLTable* SQLSession::openDataHandle(DataHandle &dh)
{
	ASSERT(currentDatabase_);
	return currentDatabase_->openDataHandle(dh);
}


void SQLSession::createIndex(const std::string& column,const std::string& table)
{
    ASSERT(currentDatabase_);
#if 0
	currentDatabase_->table(table)->column(column)->createIndex();
#endif
}

} // namespace sql
} // namespace odb
