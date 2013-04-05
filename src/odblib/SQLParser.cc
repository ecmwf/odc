/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/AutoLock.h"
#include "eclib/DataHandle.h"
#include "eclib/Exceptions.h"
#include "eclib/Mutex.h"
#include "eclib/PathName.h"
#include "eclib/Resource.h"
#include "eclib/Translator.h"

#include "odblib/oda.h"
#include "odblib/BitColumnExpression.h"
#include "odblib/ShiftedBitColumnExpression.h"
#include "odblib/ColumnExpression.h"
#include "odblib/ShiftedColumnExpression.h"
#include "odblib/Dictionary.h"
#include "odblib/FunctionExpression.h"
#include "odblib/FunctionFactory.h"
#include "odblib/NumberExpression.h"
#include "odblib/ParameterExpression.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLCreateTable.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLDistinctOutput.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLODAOutput.h"
#include "odblib/SQLOrderOutput.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLSelectFactory.h"
#include "odblib/SQLSession.h"
#include "odblib/SQLSimpleOutput.h"
#include "odblib/SQLType.h"
#include "odblib/StringExpression.h"
#include "odblib/TemporaryFile.h"

using namespace eclib;

static Mutex   mutex;
static string  yypath;

static string inputString;
char* inputText;
char* inputEnd;

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
	stringstream os;
	os << msg << " line " << odblib_lineno << " of " << yypath;
	throw SyntaxError(os.str()); 
}

}

extern "C" int odblib_wrap() { return 1; }

namespace odb {
namespace sql {

//int SQLParser::line() { using namespace SQLYacc; return SQLYacc::odblib_lineno; }

//=========================================================================

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

void SQLParser::parseString(const string& s, istream* is, SQLOutputConfig cfg, const string& csvDelimiter)
{
	AutoLock<Mutex> lock(mutex);

	SQLSelectFactory::instance().implicitFromTableSourceStream(is);
	SQLSelectFactory::instance().config(cfg);
	SQLSelectFactory::instance().csvDelimiter(csvDelimiter);

	inputString = s;
	inputText = const_cast<char *>(inputString.c_str());
	inputEnd = inputText + inputString.size();

    SQLYacc::odblib_parse();

	lexRelease();
	SQLSelectFactory::instance().implicitFromTableSourceStream(0);
}

void SQLParser::parseString(const string& s, DataHandle* dh, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
	SQLSelectFactory::instance().implicitFromTableSource(dh);
	SQLSelectFactory::instance().config(cfg);

	inputString = s;
	inputText = const_cast<char *>(inputString.c_str());
	inputEnd = inputText + inputString.size();

	SQLYacc::odblib_parse();
	lexRelease();

	SQLSelectFactory::instance().implicitFromTableSource(0);
}

void SQLParser::parseString(const string& s, SQLDatabase& db, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    SQLSession& session = SQLSession::current();
    session.currentDatabase(&db);
	SQLSelectFactory::instance().database(&db);
	SQLSelectFactory::instance().config(cfg);

	inputString = s;
	inputText = const_cast<char *>(inputString.c_str());
	inputEnd = inputText + inputString.size();

	SQLYacc::odblib_parse();
	lexRelease();

	SQLSelectFactory::instance().implicitFromTableSource(0);
}

} // namespace sql
} // namespace odb

