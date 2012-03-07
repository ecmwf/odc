/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "SQLExpression.h"
#include "ColumnExpression.h"
#include "BitColumnExpression.h"
#include "FunctionExpression.h"
#include "ParameterExpression.h"
#include "NumberExpression.h"
#include "StringExpression.h"
#include "Dictionary.h"
#include "Resource.h"
#include "StrStream.h"
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
#include "TmpFile.h"
#include "SQLType.h"
#include "SQLBitfield.h"
#include "Translator.h"
#include "SQLCreateTable.h"
#include "DataHandle.h"
#include "SQLSelectFactory.h"

static Mutex mutex;


static string        odblib_path;

/* #define _CPP_IOSTREAMS 1 */
#define  YYDEBUG      1

namespace SQLYacc {

extern int odblib_lineno;

void odblib_error(const char* msg);

using namespace odb;
using namespace odb::sql;
using namespace odb::sql::expression;
using namespace odb::sql::expression::function;

#include "sqly.c"

void odblib_error(const char* msg)
{
    StrStream os;
    os << msg << " line " << odblib_lineno << " of " << odblib_path << StrStream::ends;
    throw SeriousBug(string(os));
}

};

struct Include {

    int       lineno;
    string    path;
    FILE*     in;

    Include() : lineno(SQLYacc::odblib_lineno), path(odblib_path), in(SQLYacc::odblib_in) {}
    ~Include() {}
};

static vector<Include> includeStack;

extern "C" int odblib_wrap()
{
    if(includeStack.size() == 0)
        return 1;

    Log::info() << "End of " << odblib_path
                << " at " << SQLYacc::odblib_lineno << ", back to "
                << includeStack.back().path << " at " << includeStack.back().lineno
                << endl;

    fclose(SQLYacc::odblib_in);

    SQLYacc::odblib_lineno = includeStack.back().lineno;
    odblib_path   = includeStack.back().path;
    SQLYacc::odblib_in     = includeStack.back().in;

    includeStack.pop_back();

    return 0;
}

namespace odb {
namespace sql {

int SQLParser::line()
{
    using namespace SQLYacc;
    return SQLYacc::odblib_lineno;
}

#if 0

void SQLParser::include(const PathName& path)
{
    string p = string(path);
    if(p[0] != '/')
        p = string(PathName(odblib_path).dirName()) + "/" + string(path);

    Log::info() << "Including " << p
                << " from " << odblib_path << " at " << SQLYacc::odblib_lineno
                << endl;

    includeStack.push_back(Include());

    FILE* in = fopen64(p.c_str(),"r");
    if(!in)
        throw CantOpenFile(p);

    SQLYacc::odblib_lineno = 0;
    odblib_path   = p;
    SQLYacc::odblib_in     = in;
}
#endif


//=========================================================================

void SQLParser::parseFile(const PathName& path, DataHandle* dh) { parseFile(path, dh, odb::sql::SQLSelectFactory::instance().config()); }

void SQLParser::parseFile(const PathName& path, istream* is) { parseFile(path, is, odb::sql::SQLSelectFactory::instance().config()); }

void SQLParser::parseFile(const PathName& path, istream* is, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    includeStack.clear();

    SQLSelectFactory::instance().implicitFromTableSourceStream(is);
    SQLSelectFactory::instance().config(cfg);

    odblib_path = path;

    FILE* in = fopen64(odblib_path.c_str(),"r");
    if(!in) throw CantOpenFile(path);

    SQLYacc::odblib_lineno = 0;
    SQLYacc::odblib_in     = in;
    SQLYacc::odblib_debug  = Resource<long>("$YYDEBUG;-odblib_debug;odblib_debug", 0);

    SQLYacc::odblib_parse();

#if YY_FLEX_MAJOR_VERSION >= 2
#if YY_FLEX_MINOR_VERSION >= 5
#if YY_FLEX_SUBMINOR_VERSION >=33
    SQLYacc::odblib_lex_destroy();
#endif
#endif
#endif

    fclose(in);

    SQLSelectFactory::instance().implicitFromTableSourceStream(0);
}

void SQLParser::parseFile(const PathName& path, DataHandle* dh, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    includeStack.clear();

    SQLSelectFactory::instance().implicitFromTableSource(dh);
    SQLSelectFactory::instance().config(cfg);

    odblib_path = path;

    FILE* in = fopen64(odblib_path.c_str(),"r");
    if(!in) throw CantOpenFile(path);

    SQLYacc::odblib_lineno = 0;
    SQLYacc::odblib_in     = in;
    SQLYacc::odblib_debug  = Resource<long>("$YYDEBUG;-odblib_debug;odblib_debug", 0);

    SQLYacc::odblib_parse();

#if YY_FLEX_MAJOR_VERSION >= 2
#if YY_FLEX_MINOR_VERSION >= 5
#if YY_FLEX_SUBMINOR_VERSION >=33
    SQLYacc::odblib_lex_destroy();
#endif
#endif
#endif

    fclose(in);

    SQLSelectFactory::instance().implicitFromTableSource(0);
}

void SQLParser::parseString(const string& s, istream* is, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    TmpFile tmp;

#ifdef linux
    FILE *f = fopen64(tmp.localPath(), "w");
    ASSERT(f);
    ASSERT(!feof(f) && !ferror(f));
    size_t written = fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    ASSERT(written == 1);
    ASSERT(!feof(f) && !ferror(f));
    Log::debug() << "" << endl;
    ASSERT(0 == fclose(f));
#else
    ofstream os(tmp.localPath());
    os << s;
    os.close();
    if(!os) throw WriteError(tmp);
#endif

    return parseFile(tmp, is, cfg);
}

void SQLParser::parseString(const string& s, DataHandle* dh, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    TmpFile tmp;

#ifdef linux
    FILE *f = fopen64(tmp.localPath(), "w");
    ASSERT(f);
    ASSERT(!feof(f) && !ferror(f));
    size_t written = fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    ASSERT(written == 1);
    ASSERT(!feof(f) && !ferror(f));
    Log::debug() << "" << endl;
    ASSERT(0 == fclose(f));
#else
    ofstream os(tmp.localPath());
    os << s;
    os.close();
    if(!os) throw WriteError(tmp);
#endif

    return parseFile(tmp, dh, cfg);
}

void SQLParser::parseFile(const PathName& path, SQLDatabase& db, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    includeStack.clear();

    SQLSelectFactory::instance().database(&db);
    SQLSelectFactory::instance().config(cfg);

    odblib_path = path;

    FILE* in = fopen64(odblib_path.c_str(),"r");
    if(!in) throw CantOpenFile(path);

    SQLYacc::odblib_lineno = 0;
    SQLYacc::odblib_in     = in;
    SQLYacc::odblib_debug  = Resource<long>("$YYDEBUG;-odblib_debug;odblib_debug", 0);

    SQLYacc::odblib_parse();

#if YY_FLEX_MAJOR_VERSION >= 2
#if YY_FLEX_MINOR_VERSION >= 5
#if YY_FLEX_SUBMINOR_VERSION >=33
    SQLYacc::odblib_lex_destroy();
#endif
#endif
#endif

    fclose(in);

    SQLSelectFactory::instance().database(0);
}

void SQLParser::parseString(const string& s, SQLDatabase& db, SQLOutputConfig cfg)
{
    AutoLock<Mutex> lock(mutex);
    TmpFile tmp;

#ifdef linux
    FILE *f = fopen64(tmp.localPath(), "w");
    ASSERT(f);
    ASSERT(!feof(f) && !ferror(f));
    size_t written = fwrite(s.c_str(), strlen(s.c_str()), 1, f);
    ASSERT(written == 1);
    ASSERT(!feof(f) && !ferror(f));
    Log::debug() << "" << endl;
    ASSERT(0 == fclose(f));
#else
    ofstream os(tmp.localPath());
    os << s;
    os.close();
    if(!os) throw WriteError(tmp);
#endif

    return parseFile(tmp, db, cfg);
}

} // namespace sql
} // namespace odb

