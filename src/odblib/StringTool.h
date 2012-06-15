/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file StringTool.h
///
/// @author Piotr Kuchta, ECMWF, Oct 2010

#ifndef StringTool_H
#define StringTool_H

#include <map>
#include <set>
#include <vector>

#include "eclib/CodeLocation.h"
#include "eclib/StringTools.h"
#include "eclib/Tokenizer.h"

using namespace std;

class PathName;

namespace odb {

class StringTool {

    typedef int(*ctypeFun)(int);

public:

    static std::string readFile(const PathName fileName, bool logging = false);
	static vector<std::string> readLines(const PathName fileName, bool logging = false);

    static void trimInPlace(std::string &);

    static bool isColumnRegex(const string&);
	static bool match(const string& regex, const string&);
	static bool matchEx(const string& regex, const string&);
	static bool matchAny(const vector<string>& regs, const string&);

	static bool check(const std::string&, ctypeFun);
	static bool isInQuotes(const std::string &);
	static std::string unQuote(const std::string &);

	static double cast_as_double(const std::string &);
	static std::string double_as_string(double);
	//static string stringAsDouble(double v)

	static string int_as_double2string(double);

	static int shell(std::string cmd, const CodeLocation &where, bool assertSuccess = true);

	static double translate(const string& v);
	static string patchTimeForMars(const string& v);

};

ostream& operator<<(ostream&, const vector<string>&);

} // namespace odb 

#endif

