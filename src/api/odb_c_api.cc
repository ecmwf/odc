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

#include "sqlite3.h"

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

DataBaseImpl& database (sqlite3* db) { return reinterpret_cast<DataBaseImpl&>(*db); }

class StatementImpl {
public:
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

StatementImpl& statement (sqlite3_stmt* stmt) { return reinterpret_cast<StatementImpl&>(*stmt); }

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

int SelectImpl::column_count()
{
    return it_->columns().size();
}

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
//  SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL.
    switch (it_->columns()[iCol]->type())
    {
        case STRING:   return SQLITE_TEXT;
        case INTEGER:  return SQLITE_INTEGER;
        case BITFIELD: return SQLITE_INTEGER; // TODO?
        case REAL:     return SQLITE_FLOAT;
        case DOUBLE:   return SQLITE_FLOAT;
        default:
            return SQLITE_NULL; // TODO?
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

//SQLITE_API const char *SQLITE_STDCALL sqlite3_errmsg(sqlite3*);
const char * sqlite3_errmsg(sqlite3* db)
{
    return "sqlite3_errmsg: TODO";
}

//SQLITE_API const char *SQLITE_STDCALL sqlite3_libversion(void);
const char * sqlite3_libversion(void)
{
    return odb::ODBAPIVersion::version();
}


//SQLITE_API int SQLITE_STDCALL sqlite3_open(
//  const char *filename,   /* Database filename (UTF-8) */
//  sqlite3 **ppDb          /* OUT: SQLite db handle */
//);

int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
) 
{
    eckit::Log::info() << "Open database '" << filename << "'" << std::endl;

    typedef sqlite3 * dbp_t; 
    *ppDb = dbp_t( new DataBaseImpl(filename) );
    return SQLITE_OK;
}

//SQLITE_API int SQLITE_STDCALL sqlite3_close(sqlite3*);
int sqlite3_close(sqlite3*)
{
    //TODO
    return SQLITE_OK;
}

//SQLITE_API int SQLITE_STDCALL sqlite3_prepare_v2(
//  sqlite3 *db,            /* Database handle */
//  const char *zSql,       /* SQL statement, UTF-8 encoded */
//  int nByte,              /* Maximum length of zSql in bytes. */
//  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
//  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
//);

int sqlite3_prepare_v2(
  sqlite3 *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
)
{
    eckit::Log::info() << "Prepare statement '" << zSql << "'" << std::endl;

    odb::sql::SQLParser parser;
    // TODO: parse string, see if we get SelectStatement or InsertStatement

    typedef sqlite3_stmt* stmt_ptr_t; 
    *ppStmt = stmt_ptr_t (new SelectImpl(database(db).filename().c_str(), zSql));

    return SQLITE_OK;
}

//SQLITE_API int SQLITE_STDCALL sqlite3_step(sqlite3_stmt*)
int sqlite3_step(sqlite3_stmt* stmt)
{
    if (! stmt) 
        return SQLITE_ERROR;

    if (statement(stmt).step())
        return SQLITE_ROW;

    return SQLITE_DONE;
}

//int sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
//int sqlite3_bind_blob64(sqlite3_stmt*, int, const void*, sqlite3_uint64, void(*)(void*));
int sqlite3_bind_double(sqlite3_stmt* stmt, int i, double v)
{
    return statement(stmt).bind_double(i, v);
}

int sqlite3_bind_int(sqlite3_stmt* stmt, int i, int v)
{
    return statement(stmt).bind_int(i, v);
}

//int sqlite3_bind_int64(sqlite3_stmt*, int, sqlite3_int64);
//int sqlite3_bind_null(sqlite3_stmt*, int);
//int sqlite3_bind_text(sqlite3_stmt*,int,const char*,int,void(*)(void*));
//int sqlite3_bind_text16(sqlite3_stmt*, int, const void*, int, void(*)(void*));
//int sqlite3_bind_text64(sqlite3_stmt*, int, const char*, sqlite3_uint64, void(*)(void*), unsigned char encoding);
//int sqlite3_bind_value(sqlite3_stmt*, int, const sqlite3_value*);
//int sqlite3_bind_zeroblob(sqlite3_stmt*, int, int n);
//int sqlite3_bind_zeroblob64(sqlite3_stmt*, int, sqlite3_uint64);


//SQLITE_API const unsigned char *SQLITE_STDCALL sqlite3_column_text(sqlite3_stmt*, int iCol);
const unsigned char *sqlite3_column_text(sqlite3_stmt* stmt, int column)
{
    return statement(stmt).column_text(column);
}

//SQLITE_API int SQLITE_STDCALL sqlite3_finalize(sqlite3_stmt *pStmt);
int sqlite3_finalize(sqlite3_stmt *stmt)
{
    //TODO
    return SQLITE_OK;
}

// https://www.sqlite.org/c3ref/column_name.html
const char *sqlite3_column_name(sqlite3_stmt* stmt, int iCol)
{
    return statement(stmt).column_name(iCol);
}

//https://www.sqlite.org/c3ref/column_blob.html
int sqlite3_column_type(sqlite3_stmt* stmt, int iCol)
{
    return statement(stmt).column_type(iCol);
}

// https://www.sqlite.org/c3ref/column_count.html
int sqlite3_column_count(sqlite3_stmt *stmt)
{
    return statement(stmt).column_count();
}

} // extern "C" 

