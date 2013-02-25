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

#include "odblib/SQLOutputConfig.h"

namespace eclib { class PathName; }
namespace eclib { class DataHandle; }

namespace odb {
namespace sql {

class SyntaxError : public SeriousBug {
public:
	SyntaxError(const string& s): SeriousBug(s) {}
};

class SQLParser {
public:

	static int line();
	static void include(const eclib::PathName&);

	//static void parseFile(const eclib::PathName&, eclib::DataHandle*);
	//static void parseFile(const eclib::PathName&, eclib::DataHandle*, SQLOutputConfig);
	static void parseString(const string&, eclib::DataHandle*, SQLOutputConfig);

	//static void parseFile(const eclib::PathName&, istream*);
	//static void parseFile(const eclib::PathName&, istream*, SQLOutputConfig);
	static void parseString(const string&, istream*, SQLOutputConfig);


	//static void parseFile(const eclib::PathName&, SQLDatabase&, SQLOutputConfig);
	static void parseString(const string&, SQLDatabase&, SQLOutputConfig);
	static void lexRelease();
};

} // namespace sql
} // namespace odb

#endif
