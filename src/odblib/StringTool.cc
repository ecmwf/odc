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
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
#include <strings.h>
#include <sstream>
#include <iomanip>
#include <ctype.h>

#include "eckit/io/FileHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/utils/Regex.h"
#include "eckit/parser/Translator.h"

#include "odblib/Endian.h"
#include "odblib/MemoryBlock.h"
#include "odblib/StringTool.h"

using namespace std;
using namespace eckit;

namespace odb {

vector<string> StringTool::readLines(const PathName fileName, bool logging)
{
	string s = readFile(fileName, logging);
	return StringTools::split("\n", s);
}

std::string StringTool::readFile(const PathName fileName, bool logging)
{
	const size_t CHUNK_SIZE = 1024;
	char buffer[CHUNK_SIZE]; 

	FileHandle f(fileName);
	Length estimated = f.openForRead();
	
	std::string ret;
	size_t read, totalRead = 0;

	while ( (read = f.read(buffer, sizeof(buffer) / sizeof(char))) > 0 )
	{
		totalRead += read;
		ret.append(std::string(static_cast<char*>(buffer), read));
	}

	if (logging)
		Log::info()  << "Read " << totalRead << " bytes from file " << fileName << "[" << ret << "]" << endl;

	f.close();
	return ret;
}

int StringTool::shell(std::string cmd, const CodeLocation& where, bool assertSuccess)
{
	std::string c = "/bin/sh -c \"" + cmd + "\"";

	Log::info() << "Executing '" + c + "' ";
    Log::info() << " " << where.file() << " +" << where.line();
	Log::info() << endl;

	int rc = system(c.c_str());

	if (assertSuccess && rc != 0)
	{
		throw eckit::SeriousBug(string(" \"") + cmd + "\" failed. " + where.file() + " +" + Translator<int, string>()(where.line()));
		ASSERT(rc == 0);
	}
	return rc;
}

bool StringTool::check(const std::string& s, ctypeFun fun)
{
	for (size_t i = 0; i < s.size(); ++i)
		if (! fun(s[i]))
			return false;
	return true;
}

bool StringTool::isInQuotes(const std::string& value)
{
	return value.size() > 1
		&& ((value[0] == '"' && value[value.size() - 1] == '"')
	     || (value[0] == '\'' && value[value.size() - 1] == '\''));
}

std::string StringTool::unQuote(const std::string& value)
{
	if (! value.size())
		return value;
	if (isInQuotes(value))
		return value.substr(1, value.size() - 2);
	return value;
}

std::string StringTool::double_as_string(double m)
{
	char buf[sizeof(double) + 1];
	bzero(buf, sizeof(buf));
	memcpy(buf, reinterpret_cast<char *>(&m), sizeof(double));
	return std::string(buf, sizeof(double));
}

double StringTool::cast_as_double(const std::string& value)
{
	char buf[sizeof(double)];
	memset(buf, ' ', sizeof(double));

	ASSERT(value.size() <= sizeof(double));

	strncpy(buf + sizeof(double) - value.size(), value.c_str(), value.size());
	return *reinterpret_cast<double *>(buf);
}

string StringTool::int_as_double2string(double v)
{
    stringstream s;
    s.precision(0);
    s << fixed << v;
    return s.str();
}

double StringTool::translate(const string& v)
{
	return isInQuotes(v) ? cast_as_double(unQuote(v)) : Translator<string, double>()(v);
}

void StringTool::trimInPlace(std::string &str) { str = StringTools::trim(str); }

bool StringTool::match(const string& regex, const string& s)
{
	return Regex(regex).match(s);
}

bool StringTool::matchEx(const string& regex, const string& s)
{
	bool negated = false;	
	string rx = regex;
	if (rx[0] == '~')
	{
		rx.erase(0, 0);
		negated = true;
	}

	// TODO: remove '/' 

	bool matches = Regex(rx).match(s);
	return (negated && !matches) || (!negated && matches);
}

bool StringTool::isColumnRegex(const string& s)
{
    return ( s[0] == '/' && s[s.size() - 1] == '/' )
        || ( s[0] == '~' && s[1] == '/' && s[s.size() - 1] == '/' );
}

bool StringTool::matchAny(const vector<string>& regs, const string& s)
{
	for (size_t i = 0; i < regs.size(); ++i)
		if (match(regs[i], s))
			return true;
	return false;
}

ostream& operator<<(ostream& s, const vector<string>& st) 
{
    s << '[';
    for (vector<string>::const_iterator it = st.begin(); it != st.end(); ++it)
        s << *it << ",";
    s << ']';
    return s;
}


string StringTool::valueAsString(double d, ColumnType t)
{
	stringstream s;
	switch (t) {
	case INTEGER: return int_as_double2string(d);
	case BITFIELD: return int_as_double2string(d); // TODO: have something to print bitfields in StringTool
	case STRING: return double_as_string(d);
	case DOUBLE:
	case REAL: 
		 s << d; return s.str();
	case IGNORE:
	default:
		ASSERT(0 && "Type not known.");
	}
	s << d;
	return s.str();
}

string StringTool::patchTimeForMars(const string& ss)
{
	string v = ss;
	if (v.size() == 5) v = string("0") + v;
	if (v.size() == 6)
	{
		string s = v;
		v = v.substr(0, 4);
		Log::debug() << "StringTool::patchTimeForMars: stripping seconds from TIME: '"
				<< s << "' => '" << v << "'" << endl;
	}
	return v;
}

bool StringTool::isSelectStatement(const string& s) { return StringTool::match("select", eckit::StringTools::lower(eckit::StringTools::trim(s))); }

} // namespace odb 

