/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "odb_api/BitColumnExpression.h"
#include "odb_api/ColumnExpression.h"
#include "odb_api/Dictionary.h"
#include "odb_api/FunctionFactory.h"
#include "odb_api/NumberExpression.h"
#include "odb_api/ParameterExpression.h"
#include "odb_api/SQLAST.h"
#include "odb_api/SQLBitfield.h"
#include "odb_api/SQLDatabase.h"
#include "odb_api/SQLExpression.h"
#include "odb_api/SQLParser.h"
#include "odb_api/SQLSelectFactory.h"
#include "odb_api/SQLInsertFactory.h"
#include "odb_api/SQLSelect.h"
#include "odb_api/SQLEmbedded.h"
#include "odb_api/SQLSession.h"
#include "odb_api/StringExpression.h"
#include "odb_api/EmbeddedCodeExpression.h"
#include "odb_api/StringTool.h"
#include "odb_api/EmbeddedCodeParser.h"
#include "odb_api/FunctionMATCH.h"
#include "odb_api/SQLAST.h"

using namespace eckit;

static Mutex   local_mutex;

//char* inputText;
//char* inputEnd;

/* #define _CPP_IOSTREAMS 1 */
#define  YYDEBUG      1

namespace SQLYacc {

// Original lex defines odblib_lineno, but it's not documented, so flex does not define it (thank you, RMS).
//#ifndef AIX
//	extern
//#endif
//extern int odblib_lineno;

typedef void * odblib_scan_t;

void odblib_error(odblib_scan_t, odb::sql::SQLSession*, const char* msg);

using namespace odb;
using namespace odb::sql;
using namespace odb::sql::expression;
using namespace odb::sql::expression::function;

#include "odb_api/odblib_lex.h"
#include "odb_api/sqly.c"

void odblib_error(odblib_scan_t scanner, odb::sql::SQLSession*, const char* msg)
{
    std::stringstream os;
    // TODO: FIXME
	//os << msg << " line " << odblib_lineno; // TODO: << " of " << yypath;
	throw SyntaxError(os.str()); 
}

} // namespace SQLYacc

extern "C" int odblib_wrap(void *scanner) { return SQLYacc::wrap( (SQLYacc::odblib__buffer_state*) scanner); }

namespace odb {
namespace sql {

//int SQLParser::line() { using namespace SQLYacc; return SQLYacc::odblib_lineno; }

//void SQLParser::pushInclude(const std::string& sql, const std::string& yypath, SQLYacc::YY_BUFFER_STATE state, SQLYacc::odblib_scan_t scanner) 
void SQLParser::pushInclude(const std::string& sql, const std::string& yypath, void* state, void* scanner) 
{ 
    SQLYacc::pushInclude(sql, yypath, (SQLYacc::YY_BUFFER_STATE) state, (SQLYacc::odblib_scan_t) scanner); 
}

void SQLParser::popInclude(void* scanner) { SQLYacc::popInclude((SQLYacc::odblib_scan_t) scanner); }

struct SessionResetter {
    SessionResetter (SQLSession& s) : session_(s) {}
    ~SessionResetter () 
    {
#if YY_FLEX_MAJOR_VERSION >= 2
#if YY_FLEX_MINOR_VERSION >= 5
#if YY_FLEX_SUBMINOR_VERSION >=33
    //SQLYacc::yylex_destroy(); 
#endif
#endif
#endif
        session_.selectFactory().implicitFromTableSource(0);
        session_.selectFactory().implicitFromTableSourceStream(0);
        session_.selectFactory().database(0);
    }
private:
    SQLSession& session_;
};

void SQLParser::parseString(odb::sql::SQLSession& session, const std::string& s, std::istream* is, SQLOutputConfig cfg, const std::string& csvDelimiter)
{
    AutoLock<Mutex> lock(local_mutex);
    SessionResetter ar (session);

    //SQLYacc::odblib_lineno = 0;

    session.selectFactory().implicitFromTableSourceStream(is);
    session.selectFactory().config(cfg);
    session.selectFactory().csvDelimiter(csvDelimiter);

    SQLYacc::odblib_scan_t scanner;
    SQLYacc::odblib_lex_init(&scanner);
    // FIXME: How do we combine this with 
    // yyset_in(fopen(argv[1], "rb"), scanner);

//void SQLParser::pushInclude(const std::string& sql, const std::string& yypath, void* state, void* scanner) 
    pushInclude(s, "", scanner, scanner);
    SQLYacc::odblib_parse(scanner, &session);

    SQLYacc::odblib_lex_init(&scanner); // TODO: handle unwind
}

void SQLParser::parseString(odb::sql::SQLSession& session, const std::string& s, DataHandle* dh, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(local_mutex);
    SessionResetter ar (session);

    //SQLYacc::odblib_lineno = 0;

    session.selectFactory().implicitFromTableSource(dh);
    session.selectFactory().config(cfg);

    SQLYacc::odblib_scan_t scanner;
    SQLYacc::odblib_lex_init(&scanner);

    pushInclude(s, "", scanner, scanner);
    SQLYacc::odblib_parse(scanner, &session);
}

void SQLParser::parseString(odb::sql::SQLSession& session,const std::string& s, SQLDatabase& db, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(local_mutex);
    SessionResetter ar (session);

    //SQLYacc::odblib_lineno = 0;

    session.currentDatabase(&db);
    session.selectFactory().database(&db);
    session.selectFactory().config(cfg);

    SQLYacc::odblib_scan_t scanner;
    SQLYacc::odblib_lex_init(&scanner);
    pushInclude(s, "", scanner, scanner);
    SQLYacc::odblib_parse(scanner, &session);
}

} // namespace sql
} // namespace odb

