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
//#include "eckit/io/DataHandle.h"
//#include "eckit/exception/Exceptions.h"
#include "eckit/thread/Mutex.h"
//#include "eckit/filesystem/PathName.h"
//#include "eckit/config/Resource.h"
//#include "eckit/parser/Translator.h"

//#include "odblib/odb_api.h"
#include "odblib/BitColumnExpression.h"
//#include "odblib/ShiftedBitColumnExpression.h"
#include "odblib/ColumnExpression.h"
//#include "odblib/ShiftedColumnExpression.h"
#include "odblib/Dictionary.h"
#include "odblib/SQLAST.h"

//#include "odblib/FunctionExpression.h"
#include "odblib/FunctionFactory.h"
#include "odblib/NumberExpression.h"
#include "odblib/ParameterExpression.h"
#include "odblib/SQLBitfield.h"
//#include "odblib/SQLCreateTable.h"
#include "odblib/SQLDatabase.h"
//#include "odblib/SQLDistinctOutput.h"
//#include "odblib/SQLExpression.h"
//#include "odblib/SQLIteratorSession.h"
//#include "odblib/SQLODAOutput.h"
//#include "odblib/SQLOrderOutput.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLSelectFactory.h"
#include "odblib/SQLSession.h"
//#include "odblib/SQLSimpleOutput.h"
#include "odblib/StringTool.h"
#include "odblib/StringExpression.h"
#include "odblib/SQLExpression.h"

//#include "odblib/TemporaryFile.h"

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
extern int odblib_lineno;

void odblib_error(const char* msg);

using namespace odb;
using namespace odb::sql;
using namespace odb::sql::expression;
using namespace odb::sql::expression::function;

#include "odblib/sqly.c"

void odblib_error(const char* msg)
{
    std::stringstream os;
	os << msg << " line " << odblib_lineno; // TODO: << " of " << yypath;
	throw SyntaxError(os.str()); 
}

} // namespace SQLYacc

extern "C" int odblib_wrap() { return SQLYacc::wrap(); }

namespace odb {
namespace sql {

//int SQLParser::line() { using namespace SQLYacc; return SQLYacc::odblib_lineno; }

void SQLParser::pushInclude(const std::string& sql, const std::string& yypath) { SQLYacc::pushInclude(sql, yypath); }
void SQLParser::popInclude() { SQLYacc::popInclude(); }

void SQLParser::parseString(const std::string& s, std::istream* is, SQLOutputConfig cfg, const std::string& csvDelimiter)
{
    AutoLock<Mutex> lock(local_mutex);

	SQLSelectFactory::instance().implicitFromTableSourceStream(is);
	SQLSelectFactory::instance().config(cfg);
	SQLSelectFactory::instance().csvDelimiter(csvDelimiter);

	pushInclude(s, "");

    SQLYacc::odblib_parse();
	lexRelease();

	SQLSelectFactory::instance().implicitFromTableSourceStream(0);
}

void SQLParser::parseString(const std::string& s, DataHandle* dh, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(local_mutex);
	SQLSelectFactory::instance().implicitFromTableSource(dh);
	SQLSelectFactory::instance().config(cfg);

	pushInclude(s, "");

	SQLYacc::odblib_parse();
	lexRelease();

	SQLSelectFactory::instance().implicitFromTableSource(0);
}

void SQLParser::parseString(const std::string& s, SQLDatabase& db, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(local_mutex);
    SQLSession::current().currentDatabase(&db);
	SQLSelectFactory::instance().database(&db);
	SQLSelectFactory::instance().config(cfg);

	pushInclude(s, "");

	SQLYacc::odblib_parse();
	lexRelease();

    //SQLSession::current().currentDatabase(0);
	SQLSelectFactory::instance().implicitFromTableSource(0);
	SQLSelectFactory::instance().database(0);
}


void SQLParser::lexRelease()
{
#if YY_FLEX_MAJOR_VERSION >= 2
#if YY_FLEX_MINOR_VERSION >= 5
#if YY_FLEX_SUBMINOR_VERSION >=33
    // TODO: come back here and fix this
    //	SQLYacc::yylex_destroy(); 
#endif
#endif
#endif
}

} // namespace sql
} // namespace odb

