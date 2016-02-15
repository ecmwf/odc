/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLParser.h
// Baudouin Raoult - ECMWF Mar 98

#ifndef SQLParser_H
#define SQLParser_H

#include "eckit/exception/Exceptions.h"
//#include "odb_api/SQLOutputConfig.h"
#include "SQLOutputConfig.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }
namespace eckit { class ExecutionContext; }

namespace odb {
namespace sql {

class SQLDatabase;
class SQLSession;

class SyntaxError : public eckit::SeriousBug {
public:
	SyntaxError(const std::string& s): eckit::SeriousBug(s) {}
};

struct ParseFrame {
	ParseFrame(const std::string& sql, const std::string& yypath);

	std::string inputString_;
	std::string yypath_;
	char* inputText_;
	char* inputEnd_;
};

class SQLParser {
public:
	static int line();

    // 3 dodgy methods for compatibility with older versions
	static void parseString(const std::string&, eckit::DataHandle*, const SQLOutputConfig&);
    static void parseString(const std::string&, std::istream*, const SQLOutputConfig&, const std::string& cvsDelimiter);
	static void parseString(const std::string&, SQLDatabase&, const SQLOutputConfig&);

	static void parseString(odb::sql::SQLSession&, const std::string&, eckit::DataHandle*, const SQLOutputConfig&);
    static void parseString(odb::sql::SQLSession&, const std::string&, std::istream*, const SQLOutputConfig&, const std::string& cvsDelimiter);
	static void parseString(odb::sql::SQLSession&, const std::string&, SQLDatabase&, const SQLOutputConfig&);

	//static void include(const eckit::PathName&);

	static void pushInclude(const std::string&, const std::string&, void*, void*);
	static void popInclude(void*);

	static std::stack<ParseFrame> frames_;
};

} // namespace sql
} // namespace odb

#endif
