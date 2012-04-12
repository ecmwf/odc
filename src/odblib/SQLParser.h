/*
 * © Copyright 1996-2012 ECMWF.
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

#include "odblib/SQLOutputConfig.h"

class PathName;
class DataHandle;

namespace odb {
namespace sql {

class SyntaxError : public SeriousBug {
public:
	SyntaxError(const string& s): SeriousBug(s) {}
};

class SQLParser {
public:

	static int line();
	static void include(const PathName&);

	//static void parseFile(const PathName&, DataHandle*);
	//static void parseFile(const PathName&, DataHandle*, SQLOutputConfig);
	static void parseString(const string&, DataHandle*, SQLOutputConfig);

	//static void parseFile(const PathName&, istream*);
	//static void parseFile(const PathName&, istream*, SQLOutputConfig);
	static void parseString(const string&, istream*, SQLOutputConfig);


	//static void parseFile(const PathName&, SQLDatabase&, SQLOutputConfig);
	static void parseString(const string&, SQLDatabase&, SQLOutputConfig);
	static void lexRelease();
};

} // namespace sql
} // namespace odb

#endif
