#include "SQLExpression.h"
#include "ColumnExpression.h"
#include "BitColumnExpression.h"
#include "FunctionExpression.h"
#include "ParameterExpression.h"
#include "NumberExpression.h"
#include "StringExpression.h"
#include "Dictionary.h"
#include "Resource.h"
#include "Exceptions.h"
#include "PathName.h"

#include "oda.h"

#include "SQLParser.h"
#include "SQLDatabase.h"
#include "SQLSelect.h"
#include "SQLSimpleOutput.h"
#include "SQLDistinctOutput.h"
#include "SQLIteratorSession.h"
#include "SQLOrderOutput.h"
#include "SQLODAOutput.h"
#include "SQLSession.h"

#include "Mutex.h"
#include "AutoLock.h"
#include "TemporaryFile.h"
#include "SQLType.h"
#include "SQLBitfield.h"
#include "Translator.h"
#include "SQLCreateTable.h"
#include "DataHandle.h"
#include "SQLSelectFactory.h"

static Mutex mutex;

static string        yypath;

static string inputString;
char* inputText;
char* inputEnd;

/* #define _CPP_IOSTREAMS 1 */
#define  YYDEBUG      1

namespace SQLYacc {

// Original lex defines yylineno, but it's not documented, so flex does not define it (thank you, RMS).
#ifndef AIX
	extern
#endif
int yylineno;

void yyerror(const char* msg);

using namespace odb;
using namespace odb::sql;
using namespace odb::sql::expression;
using namespace odb::sql::expression::function;

#include "sqly.c"

void yyerror(const char* msg)
{
	stringstream os;
	os << msg << " line " << yylineno << " of " << yypath;
	throw SyntaxError(os.str()); 
}

};

extern "C" int yywrap() { return 1; }

namespace odb {
namespace sql {

//int SQLParser::line() { using namespace SQLYacc; return SQLYacc::yylineno; }

//=========================================================================

void SQLParser::lexRelease()
{
#if YY_FLEX_MAJOR_VERSION >= 2
#if YY_FLEX_MINOR_VERSION >= 5
#if YY_FLEX_SUBMINOR_VERSION >=33
	SQLYacc::yylex_destroy();
#endif
#endif
#endif
}

/*
void SQLParser::parseFile(const PathName& path, DataHandle* dh) { parseFile(path, dh, odb::sql::SQLSelectFactory::instance().config()); }

void SQLParser::parseFile(const PathName& path, istream* is) { parseFile(path, is, odb::sql::SQLSelectFactory::instance().config()); }

void SQLParser::parseFile(const PathName& path, istream* is, SQLOutputConfig cfg)
{
	AutoLock<Mutex> lock(mutex);

	SQLSelectFactory::instance().implicitFromTableSourceStream(is);
	SQLSelectFactory::instance().config(cfg);

	yypath = path;

	FILE* in = fopen64(yypath.c_str(),"r");
	if(!in) throw CantOpenFile(path);

	SQLYacc::yylineno = 0;
	SQLYacc::yyin     = in;
	SQLYacc::yydebug  = Resource<long>("$YYDEBUG;-yydebug;yydebug", 0);

	SQLYacc::yyparse();

	lexRelease();

	fclose(in);

	SQLSelectFactory::instance().implicitFromTableSourceStream(0);
}
*/

void SQLParser::parseString(const string& s, istream* is, SQLOutputConfig cfg)
{
	AutoLock<Mutex> lock(mutex);

	SQLSelectFactory::instance().implicitFromTableSourceStream(is);
	SQLSelectFactory::instance().config(cfg);

	inputString = s;
	inputText = const_cast<char *>(inputString.c_str());
	inputEnd = inputText + inputString.size();

	SQLYacc::yyparse();

	lexRelease();
	SQLSelectFactory::instance().implicitFromTableSourceStream(0);
}
/*
void SQLParser::parseFile(const PathName& path, DataHandle* dh, SQLOutputConfig cfg)
{
	AutoLock<Mutex> lock(mutex);

	SQLSelectFactory::instance().implicitFromTableSource(dh);
	SQLSelectFactory::instance().config(cfg);

	yypath = path;

	FILE* in = fopen64(yypath.c_str(),"r");
	if(!in) throw CantOpenFile(path);

	SQLYacc::yylineno = 0;
	SQLYacc::yyin     = in;
	SQLYacc::yydebug  = Resource<long>("$YYDEBUG;-yydebug;yydebug", 0);

	SQLYacc::yyparse();

	lexRelease();

	fclose(in);

	SQLSelectFactory::instance().implicitFromTableSource(0);
}
*/

void SQLParser::parseString(const string& s, DataHandle* dh, SQLOutputConfig cfg)
{
	AutoLock<Mutex> lock(mutex);
	SQLSelectFactory::instance().implicitFromTableSource(dh);
	SQLSelectFactory::instance().config(cfg);

	inputString = s;
	inputText = const_cast<char *>(inputString.c_str());
	inputEnd = inputText + inputString.size();

	SQLYacc::yyparse();
	lexRelease();

	SQLSelectFactory::instance().implicitFromTableSource(0);
}
/*
void SQLParser::parseFile(const PathName& path, SQLDatabase& db, SQLOutputConfig cfg)
{
	AutoLock<Mutex> lock(mutex);

	SQLSelectFactory::instance().database(&db);
	SQLSelectFactory::instance().config(cfg);

	yypath = path;

	FILE* in = fopen64(yypath.c_str(),"r");
	if(!in) throw CantOpenFile(path);

	SQLYacc::yylineno = 0;
	SQLYacc::yyin     = in;
	SQLYacc::yydebug  = Resource<long>("$YYDEBUG;-yydebug;yydebug", 0);

	SQLYacc::yyparse();

	lexRelease();

	fclose(in);

	SQLSelectFactory::instance().database(0);
}
*/
void SQLParser::parseString(const string& s, SQLDatabase& db, SQLOutputConfig cfg)
{
	AutoLock<Mutex> lock(mutex);
	SQLSelectFactory::instance().database(&db);
	SQLSelectFactory::instance().config(cfg);

	inputString = s;
	inputText = const_cast<char *>(inputString.c_str());
	inputEnd = inputText + inputString.size();

	SQLYacc::yyparse();
	lexRelease();

	SQLSelectFactory::instance().implicitFromTableSource(0);
}

} // namespace sql
} // namespace odb

