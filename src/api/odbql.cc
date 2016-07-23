/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file odbcapi.cc
///
/// @author Piotr Kuchta, March 2009

#include "odb_api/odb_api.h"

#include "eckit/runtime/ContextBehavior.h"
#include "eckit/runtime/Context.h"
#include "odb_api/FastODA2Request.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/ODAHandle.h"
#include "odb_api/ODBAPISettings.h"
#include "odb_api/ODBAPIVersion.h"
#include "odb_api/ODBBehavior.h"
#include "odb_api/odbcapi.h"
#include "odb_api/Select.h"
#include "odb_api/Reader.h"
#include "odb_api/Writer.h"
#include "odb_api/ColumnType.h"
#include "odb_api/SQLParser.h"
#include "odb_api/InMemoryDataHandle.h"
#include "odb_api/SQLInteractiveSession.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLOutputConfig.h"
#include "odb_api/SQLDatabase.h"

#include "odbql.h"

using namespace eckit;
using namespace odb;

extern "C" {

/////////////////////////////////////////////////////////////
// #     #                            #    ######    ###   
// ##    #  ######  #    #           # #   #     #    #    
// # #   #  #       #    #          #   #  #     #    #    
// #  #  #  #####   #    #         #     # ######     #    
// #   # #  #       # ## #         ####### #          #    
// #    ##  #       ##  ##         #     # #          #    
// #     #  ######  #    #         #     # #         ###

class DataBaseImpl {
public:
    DataBaseImpl(const char* filename)
    : filename_(filename)
    {}

    const std::string& filename() const { return filename_; }

private:
    const std::string filename_;
};

DataBaseImpl& database (odbql* db) { return reinterpret_cast<DataBaseImpl&>(*db); }

class StatementImpl {
public:
    virtual ~StatementImpl() {}

    virtual bool step() = 0;
    virtual const unsigned char *column_text(int iCol) = 0;
    virtual const char *column_name(int iCol) = 0;
    virtual int column_count() = 0;
    virtual int column_type(int i) = 0;
    virtual int bind_double(int i, double v) = 0;
    virtual int bind_int(int, int) = 0;
};

class SelectImpl : public StatementImpl {
public:
    SelectImpl(const char* db, const char* sql)
    : db_(db),
      sql_(sql),
      stmt_(std::string(db) + "; " + sql),
      it_(stmt_.begin()),
      end_(stmt_.end()),
      firstStep(true)
    {} 

    bool step();
    const unsigned char *column_text(int iCol);
    const char *column_name(int iCol);
    int column_count();
    int column_type(int i);
    int bind_double(int i, double v);
    int bind_int(int, int);

private:
    bool firstStep;
    const std::string db_;
    const std::string sql_;
    odb::Select stmt_;
    odb::Select::iterator it_;
    odb::Select::iterator end_;

    std::vector<std::string> stringCache_;
    std::vector<std::string> columnNameCache_;
};

StatementImpl& statement (odbql_stmt* stmt) { return reinterpret_cast<StatementImpl&>(*stmt); }

class InsertImpl : public StatementImpl {
public:
    InsertImpl(const odb::MetaData& metaData, const std::string& location);

    bool step();
    const unsigned char *column_text(int iCol) { NOTIMP; }
    const char *column_name(int iCol) { NOTIMP; }
    int column_count(); 
    int column_type(int i) { NOTIMP; }
    int bind_double(int i, double v);
    int bind_int(int, int);

private:
    odb::Writer<> writer_;
    odb::Writer<>::iterator it_;
};

InsertImpl::InsertImpl(const odb::MetaData& metaData, const std::string& location)
: writer_(location),
  it_(writer_.begin())
{
    it_->columns(metaData);
    it_->writeHeader();
}

int InsertImpl::bind_double(int i, double v) 
{ 
    (*it_)[i] = v;
    return ODBQL_OK;
}

int InsertImpl::bind_int(int i, int v) 
{ 
    (*it_)[i] = v;
    return ODBQL_OK;
}

bool InsertImpl::step()
{
    ++it_;
    return true;
}

int InsertImpl::column_count() { return it_->columns().size(); }

bool SelectImpl::step()
{
    if (firstStep)
    {
        firstStep = false;
        return it_ != end_;
    }

    if ( !(it_ != end_))
        return false;
    else
    {
        ++it_;
        return true;
    }
}

int SelectImpl::column_count() { return it_->columns().size(); }

const unsigned char *SelectImpl::column_text(int iCol)
{
    if (iCol + 1 > stringCache_.size())
        stringCache_.resize(iCol + 1);

    if (it_->columns()[iCol]->type() == odb::STRING)
        stringCache_[iCol] = it_->string(iCol);
    else 
    {
        stringstream ss;
        ss << it_->data(iCol);
        stringCache_[iCol] = ss.str();
    }

    typedef const unsigned char * cucp_t;
    return cucp_t(stringCache_[iCol].c_str());
}

const char *SelectImpl::column_name(int iCol)
{
    if (iCol + 1 > columnNameCache_.size())
        columnNameCache_.resize(iCol + 1);

    columnNameCache_[iCol] = it_->columns()[iCol]->name();
    return columnNameCache_[iCol].c_str();
}

int SelectImpl::column_type(int iCol)
{
//  ODBQL_INTEGER, ODBQL_FLOAT, ODBQL_TEXT, ODBQL_BLOB, or ODBQL_NULL.
    switch (it_->columns()[iCol]->type())
    {
        case STRING:   return ODBQL_TEXT;
        case INTEGER:  return ODBQL_INTEGER;
        case BITFIELD: return ODBQL_INTEGER; // TODO?
        case REAL:     return ODBQL_FLOAT;
        case DOUBLE:   return ODBQL_FLOAT;
        default:
            return ODBQL_NULL; // TODO?
    }
}

int SelectImpl::bind_double(int i, double v)
{
    NOTIMP;
}

int SelectImpl::bind_int(int i, int v)
{
    NOTIMP;
}

//ODBQL_API const char *ODBQL_STDCALL odbql_errmsg(odbql*);
const char * odbql_errmsg(odbql* db)
{
    return "odbql_errmsg: TODO";
}

//ODBQL_API const char *ODBQL_STDCALL odbql_libversion(void);
const char * odbql_libversion(void)
{
    return odb::ODBAPIVersion::version();
}


//ODBQL_API int ODBQL_STDCALL odbql_open(
//  const char *filename,   /* Database filename (UTF-8) */
//  odbql **ppDb          /* OUT: SQLite db handle */
//);

int odbql_open(
  const char *filename,   /* Database filename (UTF-8) */
  odbql **ppDb          /* OUT: SQLite db handle */
) 
{
    eckit::Log::info() << "Open database '" << filename << "'" << std::endl;

    typedef odbql * dbp_t; 
    *ppDb = dbp_t( new DataBaseImpl(filename) );
    return ODBQL_OK;
}

//ODBQL_API int ODBQL_STDCALL odbql_close(odbql*);
int odbql_close(odbql*)
{
    //TODO
    return ODBQL_OK;
}

//ODBQL_API int ODBQL_STDCALL odbql_prepare_v2(
//  odbql *db,            /* Database handle */
//  const char *zSql,       /* SQL statement, UTF-8 encoded */
//  int nByte,              /* Maximum length of zSql in bytes. */
//  odbql_stmt **ppStmt,  /* OUT: Statement handle */
//  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
//);

int odbql_prepare_v2(
  odbql *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  odbql_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
)
{
    eckit::Log::info() << "Prepare statement '" << zSql << "'" << std::endl;

    odb::sql::SQLNonInteractiveSession session;
    odb::sql::SQLOutputConfig config (session.selectFactory().config());
    odb::sql::SQLParser parser;
    // Parse the schema / database
    odb::InMemoryDataHandle input;
    input.openForRead();
    //if (database(db).filename().size())
    //    parser.parseString(session, database(db).filename(), &input, config);
    parser.parseString(session, database(db).filename() + ";" + zSql, &input, config);
    odb::sql::SQLStatement* statement (session.statement());

    typedef odbql_stmt* stmt_ptr_t; 

    if (dynamic_cast<odb::sql::SQLInsert*>(statement))
    {
        odb::sql::SQLInsert& sqlInsert (dynamic_cast<odb::sql::SQLInsert&>(*statement));
        const odb::sql::Table& table (sqlInsert.table());
        const odb::sql::TableDef& tableDef (session.currentDatabase().schemaAnalyzer().findTable(table.name));

        const MetaData md (odb::sql::SQLSelectFactory::toODAColumns(session, tableDef));
        const std::string& location (tableDef.location());

        *ppStmt = stmt_ptr_t (new InsertImpl(md, location));
        return ODBQL_OK;
    }
  
    // Assume this is SELECT for now 
    *ppStmt = stmt_ptr_t (new SelectImpl(database(db).filename().c_str(), zSql));

    return ODBQL_OK;
}

//ODBQL_API int ODBQL_STDCALL odbql_step(odbql_stmt*)
int odbql_step(odbql_stmt* stmt)
{
    if (! stmt) 
        return ODBQL_ERROR;

    if (statement(stmt).step())
        return ODBQL_ROW;

    return ODBQL_DONE;
}

//int odbql_bind_blob(odbql_stmt*, int, const void*, int n, void(*)(void*));
//int odbql_bind_blob64(odbql_stmt*, int, const void*, odbql_uint64, void(*)(void*));
int odbql_bind_double(odbql_stmt* stmt, int i, double v)
{
    return statement(stmt).bind_double(i, v);
}

int odbql_bind_int(odbql_stmt* stmt, int i, int v)
{
    return statement(stmt).bind_int(i, v);
}

//int odbql_bind_int64(odbql_stmt*, int, odbql_int64);
int odbql_bind_null(odbql_stmt*, int)
{
    // TODO
    NOTIMP;
}

int odbql_bind_text(odbql_stmt*,int,const char*,int,void(*)(void*))
{
    // TODO
    NOTIMP;
}

//int odbql_bind_text16(odbql_stmt*, int, const void*, int, void(*)(void*));
//int odbql_bind_text64(odbql_stmt*, int, const char*, odbql_uint64, void(*)(void*), unsigned char encoding);
//int odbql_bind_value(odbql_stmt*, int, const odbql_value*);
//int odbql_bind_zeroblob(odbql_stmt*, int, int n);
//int odbql_bind_zeroblob64(odbql_stmt*, int, odbql_uint64);


//ODBQL_API const unsigned char *ODBQL_STDCALL odbql_column_text(odbql_stmt*, int iCol);
const unsigned char *odbql_column_text(odbql_stmt* stmt, int column)
{
    return statement(stmt).column_text(column);
}

//ODBQL_API int ODBQL_STDCALL odbql_finalize(odbql_stmt *pStmt);
int odbql_finalize(odbql_stmt *stmt)
{
    delete &statement(stmt);
    return ODBQL_OK;
}

// https://www.sqlite.org/c3ref/column_name.html
const char *odbql_column_name(odbql_stmt* stmt, int iCol)
{
    return statement(stmt).column_name(iCol);
}

//https://www.sqlite.org/c3ref/column_blob.html
int odbql_column_type(odbql_stmt* stmt, int iCol)
{
    return statement(stmt).column_type(iCol);
}

// https://www.sqlite.org/c3ref/column_count.html
int odbql_column_count(odbql_stmt *stmt)
{
    return statement(stmt).column_count();
}

} // extern "C" 

