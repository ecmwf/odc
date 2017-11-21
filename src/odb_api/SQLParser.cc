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
#include "odb_api/SQLInsertFactory.h"
#include "odb_api/SQLSelect.h"
#include "odb_api/SQLSession.h"
#include "odb_api/StringExpression.h"
#include "odb_api/EmbeddedCodeExpression.h"
#include "odb_api/StringTool.h"
#include "odb_api/EmbeddedCodeParser.h"
#include "odb_api/FunctionMATCH.h"
#include "odb_api/SQLAST.h"

using namespace eckit;

#define  YYDEBUG      1

namespace SQLYacc {

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

    struct odblib_guts_t * odblib_g = (struct odblib_guts_t*) scanner;
    // internally we count the lines from 0...
    int lineNumber (1 + odblib_g->odblib_lineno_r);

    os << "SQL "
	<< (msg ? msg : "syntax error") 
    << ", line " << lineNumber // << " of " << yypath;
    << ". See https://software.ecmwf.int/wiki/display/ODBAPI/SQL\n";
	throw SyntaxError(os.str()); 
}

} // namespace SQLYacc

SQLYacc::Stack& includeStack(void* odblib_scanner)
{
    SQLYacc::Stack* stack (static_cast<SQLYacc::Stack*>(((struct SQLYacc::odblib_guts_t*) odblib_scanner)->odblib_extra_r));
    ASSERT (stack);
    return *stack;
}

extern "C" int odblib_wrap(void *scanner)
{ 
    return includeStack(scanner).pop(scanner); 
}

namespace odb {
namespace sql {

struct SessionResetter {
    SessionResetter (SQLSession& s) : session_(s), resetSession_(true) {}
    SessionResetter (SQLSession& s, bool r) : session_(s), resetSession_(r) {}
    ~SessionResetter () 
    {
        if (! resetSession_)
            return;
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
    bool resetSession_;
};

void SQLParser::parseString(odb::sql::SQLSession& session, const std::string& s, std::istream* is, SQLOutputConfig cfg, const std::string& csvDelimiter)
{
    SessionResetter ar (session);

    session.selectFactory().implicitFromTableSourceStream(is);
    session.selectFactory().config(cfg);
    session.selectFactory().csvDelimiter(csvDelimiter);

    SQLYacc::odblib_scan_t scanner;
    SQLYacc::odblib_lex_init(&scanner);

    SQLYacc::include_stack stack;
    SQLYacc::odblib_lex_init_extra(&stack, &scanner);

    stack.push(cleanUpSQLText(s), "", (SQLYacc::YY_BUFFER_STATE) scanner, (SQLYacc::odblib_scan_t) scanner);
    SQLYacc::odblib_parse(scanner, &session);

    session.statement();
    session.interactive();

    SQLYacc::odblib_lex_init(&scanner); // TODO: handle unwind
}

void SQLParser::parseString(odb::sql::SQLSession& session, const std::string& s, DataHandle* dh, SQLOutputConfig cfg, bool resetSession)
{
    SessionResetter ar (session, resetSession);

    session.selectFactory().implicitFromTableSource(dh);
    session.selectFactory().config(cfg);

    SQLYacc::odblib_scan_t scanner;
    SQLYacc::odblib_lex_init(&scanner);

    SQLYacc::include_stack stack;
    SQLYacc::odblib_lex_init_extra(&stack, &scanner);

    stack.push(cleanUpSQLText(s), "", (SQLYacc::YY_BUFFER_STATE) scanner, (SQLYacc::odblib_scan_t) scanner);
    SQLYacc::odblib_parse(scanner, &session);

    session.statement();
    session.interactive();
}

void SQLParser::parseString(odb::sql::SQLSession& session,const std::string& s, SQLDatabase& db, SQLOutputConfig cfg)
{
    SessionResetter ar (session);

    session.currentDatabase(&db);
    session.selectFactory().database(&db);
    session.selectFactory().config(cfg);

    SQLYacc::odblib_scan_t scanner;
    SQLYacc::odblib_lex_init(&scanner);

    SQLYacc::include_stack stack;
    SQLYacc::odblib_lex_init_extra(&stack, &scanner);

    stack.push(cleanUpSQLText(s), "", (SQLYacc::YY_BUFFER_STATE) scanner, (SQLYacc::odblib_scan_t) scanner);
    SQLYacc::odblib_parse(scanner, &session);

    session.statement();
    session.interactive();
}

std::string SQLParser::cleanUpSQLText(const std::string& sql) {

    if (sql.empty()) return sql;

    std::string s(sql);

    StringTool::trimInPlace(s);
    if (StringTool::isInQuotes(s)) {
        s = StringTool::unQuote(s);
    }
    StringTool::trimInPlace(s);

    if (!s.empty() && *s.rbegin() != ';') {
        s.push_back(';');
    }

    return s;
}

} // namespace sql
} // namespace odb

