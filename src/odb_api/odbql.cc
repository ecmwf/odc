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
/// \file odbql.cc
///
/// @author Piotr Kuchta, July 2016

#include "odb_api/odb_api.h"

#include "eckit/io/MultiHandle.h"
#include "eckit/exception/Exceptions.h"

#include "odb_api/FastODA2Request.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/ODAHandle.h"
#include "odb_api/ODBAPISettings.h"
#include "odb_api/ODBAPIVersion.h"
#include "odb_api/odbcapi.h"
#include "odb_api/Select.h"
#include "odb_api/Reader.h"
#include "odb_api/Writer.h"
#include "odb_api/ColumnType.h"
#include "odb_api/SQLParser.h"
#include "odb_api/InMemoryDataHandle.h"
#include "odb_api/SQLInteractiveSession.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLIteratorSession.h"
#include "odb_api/SQLOutputConfig.h"
#include "odb_api/SQLDatabase.h"
#include "ecml/data/DataHandleFactory.h"

#include "odbql.h"

using namespace eckit;
using namespace odb;

/////////////////////////////////////////////////////////////
// #     #                            #    ######    ###   
// ##    #  ######  #    #           # #   #     #    #    
// # #   #  #       #    #          #   #  #     #    #    
// #  #  #  #####   #    #         #     # ######     #    
// #   # #  #       # ## #         ####### #          #    
// #    ##  #       ##  ##         #     # #          #    
// #     #  ######  #    #         #     # #         ###


typedef int error_code_t;
typedef odbql * p_odbql;

class DataBaseImpl {
public:
    DataBaseImpl(const char* filename)
    : session_(odb::sql::SQLOutputConfig::defaultConfig(), ","),
      filename_(filename),
      input_( (filename && strlen(filename))
             ? static_cast<DataHandle*>(ecml::DataHandleFactory::openForRead(filename))
             : static_cast<DataHandle*>(new odb::InMemoryDataHandle() ))
    {
        if (filename && strlen(filename))
            eckit::Log::info() << "Open file '" << filename << "'" << std::endl;
    }

    ~DataBaseImpl() 
    {
        input_->close();
        delete input_;
    }

    eckit::DataHandle* input() { return input_; }
    const char * errmsg() { return errmsg_.c_str(); }
    void errmsg(const std::string& s) { errmsg_ = s; }
    error_code_t error_code(error_code_t e) { return error_code_ = e; }
    error_code_t error_code() { return error_code_; }
    odb::sql::SQLNonInteractiveSession& session() { return session_; }

private:
    odb::sql::SQLNonInteractiveSession session_;
    const std::string filename_;
    eckit::DataHandle* input_;
    std::string errmsg_;
    error_code_t error_code_;

    friend std::ostream& operator<<(std::ostream& s, const DataBaseImpl& p)
    {
        s << "[database@" << &p << ", session: " << p.session_ << "]" << std::endl;
        return s;
    }
};

DataBaseImpl& database (odbql* db) { return reinterpret_cast<DataBaseImpl&>(*db); }

class StatementImpl {
public:
    StatementImpl(DataBaseImpl& db) : db_(db) {}

    virtual ~StatementImpl() {}

    virtual int step() = 0;
    virtual const unsigned char *column_text(int iCol) = 0;
    virtual const char *column_name(int iCol) = 0;
    virtual int column_count() = 0;
    virtual int column_type(int iCol) = 0;
    virtual error_code_t bind_double(int iCol, double v) = 0;
    virtual error_code_t bind_int(int iCol, int) = 0;
    virtual error_code_t bind_text(int iCol, const char*, int) = 0;

    // NULL handling functions:
    virtual error_code_t bind_null(int iCol) = 0;
    virtual odbql_value* column_value(int iCol) = 0; 

    DataBaseImpl& database() { return db_; }

private:
    DataBaseImpl& db_;
};

class SelectImpl : public StatementImpl {
public:
    SelectImpl(DataBaseImpl& db, const char* sql)
    : StatementImpl(db),
      sql_(sql),
      stmt_(sql, db.session()),
      //stmt_(/*db.filename() + ";\n" +*/ sql),
      it_(stmt_.begin()),
      end_(stmt_.end()),
      firstStep(true)
    {} 
    ~SelectImpl() {}

    int step();
    const unsigned char *column_text(int iCol);
    const char *column_name(int iCol);
    int column_count();
    int column_type(int iCol);
    error_code_t bind_double(int iCol, double v);
    error_code_t bind_int(int iCol, int);
    error_code_t bind_text(int iCol, const char*, int);
    error_code_t bind_null(int iCol);
    virtual odbql_value* column_value(int iCol); 
 
private:
    bool firstStep;
    const std::string sql_;
    odb::Select stmt_;
    odb::Select::iterator it_;
    odb::Select::iterator end_;

    std::vector<std::string> stringCache_;
    std::vector<std::string> columnNameCache_;
};

class SelectAllImpl : public StatementImpl {
public:
    SelectAllImpl(DataBaseImpl& db, const std::vector<std::string>& descriptors)
    : StatementImpl(db),
      dh_(openForRead(descriptors)),
      stmt_(dh_),
      it_(stmt_.end()),
      end_(stmt_.end()),
      firstStep(true)
    {} 

    ~SelectAllImpl() {}

    static std::vector<eckit::DataHandle*> openForRead(const std::vector<std::string> descriptors)
    {
        std::vector<eckit::DataHandle*> r;
        for (size_t i(0); i < descriptors.size(); ++i)
            r.push_back( ecml::DataHandleFactory::openForRead(descriptors[i]));
        return r;
    }

    int step();
    const unsigned char *column_text(int iCol);
    const char *column_name(int iCol);
    int column_count();
    int column_type(int iCol);
    error_code_t bind_double(int iCol, double v);
    error_code_t bind_int(int iCol, int);
    error_code_t bind_text(int iCol, const char*, int);
    error_code_t bind_null(int iCol);
    virtual odbql_value* column_value(int iCol); 
 
private:
    bool firstStep;
    eckit::MultiHandle dh_;
    odb::Reader stmt_;
    odb::Reader::iterator it_;
    odb::Reader::iterator end_;

    std::vector<std::string> stringCache_;
    std::vector<std::string> columnNameCache_;

    odb::MetaData currentMetaData_;
};

StatementImpl& statement (odbql_stmt* stmt) { return reinterpret_cast<StatementImpl&>(*stmt); }

class InsertImpl : public StatementImpl {
public:
    InsertImpl(DataBaseImpl&, const odb::MetaData& metaData, const std::string& into);
    ~InsertImpl();

    int step();
    const unsigned char *column_text(int iCol) { NOTIMP; }
    const char *column_name(int iCol) { NOTIMP; }
    int column_count(); 
    int column_type(int iCol) { NOTIMP; }
    error_code_t bind_double(int iCol, double v);
    error_code_t bind_int(int iCol, int);
    error_code_t bind_text(int iCol, const char*, int);
    error_code_t bind_null(int iCol);
    virtual odbql_value* column_value(int iCol);

private:
    FileHandle* fileHandle_;
    odb::Writer<> writer_;
    odb::Writer<>::iterator it_;
};

// INSERT implementation

InsertImpl::InsertImpl(DataBaseImpl& db, const odb::MetaData& metaData, const std::string& into)
: StatementImpl(db),
  fileHandle_(new FileHandle(into)),
  writer_(fileHandle_, true, true),
  it_(writer_.begin())
{
    it_->columns(metaData);
    it_->writeHeader();
}

InsertImpl::~InsertImpl()
{
    it_->close();
    fileHandle_->close();
}

error_code_t InsertImpl::bind_double(int iCol, double v) 
{ 
    (*it_)[iCol] = v;
    return ODBQL_OK;
}

error_code_t InsertImpl::bind_int(int iCol, int v) 
{ 
    (*it_)[iCol] = v;
    return ODBQL_OK;
}

error_code_t InsertImpl::bind_text(int iCol, const char* s, int n)
{ 
    if (n > sizeof(double))
    {
        // this is the current limitation...
        // TODO: set error messqge
        return ODBQL_ERROR;
    }

    char v[sizeof(double) + 1];

    memset(v, ' ', sizeof(double));
    v[sizeof(double)] = 0;

    for (size_t j(0); j < n; ++j)
        v[sizeof(double) - n + j] = s[j];

    (*it_)[iCol] = *reinterpret_cast<double*>(&v);
    return ODBQL_OK;
}

error_code_t InsertImpl::bind_null(int iCol)
{
    (*it_)[iCol] = it_->columns()[iCol]->missingValue();
    return ODBQL_OK;
}

odbql_value* InsertImpl::column_value(int iCol)
{
    if ((*it_)[iCol] == it_->columns()[iCol]->missingValue())
        return 0;

    typedef odbql_value* odbql_value_p;
    return odbql_value_p(&(*it_)[iCol]);
}

int InsertImpl::step()
{
    ++it_;
    return ODBQL_ROW;
}

int InsertImpl::column_count() { return it_->columns().size(); }

// SELECT implementation

int SelectImpl::step()
{
    if (firstStep)
    {
        firstStep = false;
        return it_ != end_ ? ODBQL_ROW : ODBQL_DONE;
    }

    if (! (it_ != end_)) 
        return ODBQL_DONE;
    else
    {
        ++it_;
        return it_ != end_ ? ODBQL_ROW : ODBQL_DONE;
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
        case BITFIELD: return ODBQL_BITFIELD;
        case REAL:     return ODBQL_FLOAT;
        case DOUBLE:   return ODBQL_FLOAT;
        default:
            return ODBQL_NULL; // TODO?
    }
}

error_code_t SelectImpl::bind_double(int iCol, double v)
{
    NOTIMP;
}

error_code_t SelectImpl::bind_int(int iCol, int v)
{
    NOTIMP;
}

error_code_t SelectImpl::bind_text(int iCol, const char* s, int n)
{
    NOTIMP;
}

error_code_t SelectImpl::bind_null(int iCol)
{
    NOTIMP;
}

odbql_value* SelectImpl::column_value(int iCol)
{
    if ((*it_)[iCol] == it_->columns()[iCol]->missingValue())
        return 0;

    typedef odbql_value* odbql_value_p;
    return odbql_value_p(&(*it_)[iCol]);
}

// 'SELECT ALL *' implementation

int SelectAllImpl::step()
{
    if (firstStep)
    {
        if (! (it_ != end_))
        {
            dh_.openForRead();
            if (dh_.estimate() == eckit::Length(0))
                return ODBQL_DONE;
            it_ = stmt_.begin();
            currentMetaData_ = it_->columns();
        }
        firstStep = false;
        return it_ != end_ ? ODBQL_ROW : ODBQL_DONE;
    }

    if (! (it_ != end_)) return ODBQL_DONE;
    else
    {
        ++it_;

        if (it_->isNewDataset() && currentMetaData_ != it_->columns())
        {
            currentMetaData_ = it_->columns();
            return ODBQL_METADATA_CHANGED;
        }

        return it_ != end_ ? ODBQL_ROW : ODBQL_DONE;
    }

}

int SelectAllImpl::column_count() 
{ 
    if (! (it_ != end_))
    {
        dh_.openForRead();
        if (dh_.estimate() == eckit::Length(0))
            return 0;
        it_ = stmt_.begin();
        currentMetaData_ = it_->columns();
    }
    return it_->columns().size(); 
}

const unsigned char *SelectAllImpl::column_text(int iCol)
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

const char *SelectAllImpl::column_name(int iCol)
{
    if (iCol + 1 > columnNameCache_.size())
        columnNameCache_.resize(iCol + 1);

    columnNameCache_[iCol] = it_->columns()[iCol]->name();
    return columnNameCache_[iCol].c_str();
}

int SelectAllImpl::column_type(int iCol)
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

error_code_t SelectAllImpl::bind_double(int iCol, double v)
{
    NOTIMP;
}

error_code_t SelectAllImpl::bind_int(int iCol, int v)
{
    NOTIMP;
}

error_code_t SelectAllImpl::bind_text(int iCol, const char* s, int n)
{
    NOTIMP;
}

error_code_t SelectAllImpl::bind_null(int iCol)
{
    NOTIMP;
}

odbql_value* SelectAllImpl::column_value(int iCol)
{
    if ((*it_)[iCol] == it_->columns()[iCol]->missingValue())
        return 0;

    typedef odbql_value* odbql_value_p;
    return odbql_value_p(&(*it_)[iCol]);
}


#define TRY_WITH_DB(d) do { DataBaseImpl *p (d); try { 
 
#define CATCH_ALL  \
    } \
    catch(const odb::sql::SyntaxError &e)  { if (p) p->errmsg("syntax error"); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(const eckit::CantOpenFile &e) { if (p) p->errmsg(e.what()); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(const eckit::ShortFile &e)    { if (p) p->errmsg(e.what()); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(const eckit::ReadError &e)    { if (p) p->errmsg(e.what()); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(const eckit::UserError &e)    { if (p) p->errmsg(e.what()); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(const eckit::Exception &e)    { if (p) p->errmsg(e.what()); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(const std::exception &e)      { if (p) p->errmsg(e.what()); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    catch(...) { if (p) p->errmsg("unknown error"); return p ? p->error_code(ODBQL_ERROR) : ODBQL_ERROR; } \
    } while(false); 


extern "C" {
//ODBQL_API const char *ODBQL_STDCALL odbql_errmsg(odbql*);
const char * odbql_errmsg(odbql* db)
{
    return database(db).errmsg();
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

error_code_t odbql_open(const char *filename, odbql **ppDb) 
{
    static char* argv[] = { const_cast<char *>("odbql"), 0 };
    odb_start_with_args(1, argv);

    TRY_WITH_DB (0)

    (*ppDb) = p_odbql( p = new  DataBaseImpl(filename) );
    return ODBQL_OK;

    CATCH_ALL
}

//ODBQL_API int ODBQL_STDCALL odbql_close(odbql*);
error_code_t odbql_close(odbql* db)
{
    delete &database(db);
    return ODBQL_OK;
}

//ODBQL_API int ODBQL_STDCALL odbql_prepare_v2(
//  odbql *db,            /* Database handle */
//  const char *zSql,       /* SQL statement, UTF-8 encoded */
//  int nByte,              /* Maximum length of zSql in bytes. */
//  odbql_stmt **ppStmt,  /* OUT: Statement handle */
//  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
//);

error_code_t odbql_prepare_v2(odbql *db, const char *zSql, int nByte, odbql_stmt **ppStmt, const char **pzTail)
{
    TRY_WITH_DB(&database(db))

    //eckit::Log::info() << "Prepare statement '" << zSql << "' db = " << database(db) << std::endl;

    odb::sql::SQLParser parser;
    odb::sql::SQLNonInteractiveSession& session (database(db).session());
    odb::sql::SQLOutputConfig config (session.selectFactory().config());
    parser.parseString(session, zSql, database(db).input(), config, false);
    odb::sql::SQLStatement* statement (session.statement());

    typedef odbql_stmt* stmt_ptr_t; 

    if (dynamic_cast<odb::sql::SQLInsert*>(statement))
    {
        odb::sql::SQLInsert& sqlInsert (dynamic_cast<odb::sql::SQLInsert&>(*statement));
        const odb::sql::Table& table (sqlInsert.table());
        const odb::sql::TableDef& tableDef (session.currentDatabase().schemaAnalyzer().findTable(table.name));

        const MetaData md (odb::sql::SQLSelectFactory::toODAColumns(session, tableDef));
        const std::string& location (tableDef.location());

        *ppStmt = stmt_ptr_t (new InsertImpl(database(db), md, location));
    }
    else if (dynamic_cast<odb::sql::SQLSelect*>(statement))
    {
        odb::sql::SQLSelect* select (dynamic_cast<odb::sql::SQLSelect*>(statement));

        if (select->all())
        {
            const std::vector<odb::sql::SQLTable*>& tables (select->tables());
            if (tables.size() != 1)
                throw UserError("Only one table required in FROM clause of 'SELECT ALL *'");
            if (select->where())
                throw UserError("'SELECT ALL *' cannot have WHERE clause yet");
            const odb::sql::SQLTable& from (*tables[0]);
            std::vector<std::string> paths;
            paths.push_back (from.path());
            *ppStmt = stmt_ptr_t (new SelectAllImpl(database(db), paths));
        }
        else
        {
            // Assume this is a regular SELECT for now 
            *ppStmt = stmt_ptr_t (new SelectImpl(database(db), zSql));
        }
    }

    return ODBQL_OK;

    CATCH_ALL
}

//ODBQL_API int ODBQL_STDCALL odbql_step(odbql_stmt*)
error_code_t odbql_step(odbql_stmt* stmt)
{
    TRY_WITH_DB(&statement(stmt).database())

    if (! stmt) 
        return ODBQL_ERROR;

    return statement(stmt).step();

    CATCH_ALL
}

// The last argument of odbql_bind_blob and similar: https://www.sqlite.org/c3ref/c_static.html
//error_code_t odbql_bind_blob(odbql_stmt*, int, const void*, int n, void(*)(void*));
//error_code_t odbql_bind_blob64(odbql_stmt*, int, const void*, odbql_uint64, void(*)(void*));
error_code_t odbql_bind_double(odbql_stmt* stmt, int iCol, double v)
{
    TRY_WITH_DB(&statement(stmt).database())
    return statement(stmt).bind_double(iCol, v);
    CATCH_ALL
}

error_code_t odbql_bind_int(odbql_stmt* stmt, int iCol, int v)
{
    TRY_WITH_DB(&statement(stmt).database())
    return statement(stmt).bind_int(iCol, v);
    CATCH_ALL
}

//int odbql_bind_int64(odbql_stmt*, int, odbql_int64);
error_code_t odbql_bind_null(odbql_stmt* stmt, int iCol)
{
    TRY_WITH_DB(&statement(stmt).database())
    return statement(stmt).bind_null(iCol);
    CATCH_ALL
}

error_code_t odbql_bind_text(odbql_stmt* stmt, int iCol, const char* s, int n, void(*d)(void*))
{
    TRY_WITH_DB(&statement(stmt).database())
    return statement(stmt).bind_text(iCol, s, n);
    CATCH_ALL
}

//error_code_t odbql_bind_text16(odbql_stmt*, int, const void*, int, void(*)(void*));
//error_code_t odbql_bind_text64(odbql_stmt*, int, const char*, odbql_uint64, void(*)(void*), unsigned char encoding);
//error_code_t odbql_bind_value(odbql_stmt*, int, const odbql_value*);
//error_code_t odbql_bind_zeroblob(odbql_stmt*, int, int n);
//error_code_t odbql_bind_zeroblob64(odbql_stmt*, int, odbql_uint64);


//ODBQL_API const unsigned char *ODBQL_STDCALL odbql_column_text(odbql_stmt*, int iCol);
const unsigned char *odbql_column_text(odbql_stmt* stmt, int iCol)
{
    return statement(stmt).column_text(iCol);
}

//ODBQL_API int ODBQL_STDCALL odbql_finalize(odbql_stmt *pStmt);
error_code_t odbql_finalize(odbql_stmt *stmt)
{
    if (stmt == 0)
        return ODBQL_OK;
  
    TRY_WITH_DB(&statement(stmt).database())
    delete &statement(stmt);
    return ODBQL_OK;
    CATCH_ALL
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

/// Use odbql_column_value to check if value of the current row of column iCol is NULL.
/// @returns  0 is the value is NULL
///           any other value means the value is not NULL
odbql_value *odbql_column_value(odbql_stmt* stmt, int iCol)
{
    return reinterpret_cast<odbql_value*>( statement(stmt).column_value(iCol) );
}

// https://www.sqlite.org/c3ref/column_count.html
int odbql_column_count(odbql_stmt *stmt)
{
    return stmt ? statement(stmt).column_count() : 0;
}

double odbql_value_double(odbql_value* vp)
{
    typedef double* double_p; 
    return *double_p(vp);
}

int odbql_value_int(odbql_value* vp)
{
    typedef double* double_p; 
    return *double_p(vp);
}

} // extern "C" 

