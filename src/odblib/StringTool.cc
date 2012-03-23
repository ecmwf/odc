/*
 * © Copyright 1996-2012 ECMWF.
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

using namespace std;

#include "eclib/Application.h"
#include "eclib/FileHandle.h"
#include "eclib/PathName.h"
#include "eclib/Regex.h"
#include "eclib/Translator.h"

#include "odblib/Endian.h"
#include "odblib/MemoryBlock.h"
#include "odblib/StringTool.h"

namespace odb {

vector<string> StringTool::readLines(const PathName fileName, bool logging)
{
	string s = readFile(fileName, logging);
	return split("\n", s);
}

std::string StringTool::readFile(const PathName fileName, bool logging)
{
	const size_t CHUNK_SIZE = 1024;
	char buffer[CHUNK_SIZE]; 

	FileHandle f(fileName, "r");
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

int StringTool::ksh(std::string cmd, const char *file, int line, bool assertSuccess)
{
	std::string c = "ksh -c \"" + cmd + "\"";

	Log::info() << "Executing '" + c + "' ";
	if (file) Log::info() << " " << file << " +" << line;
	Log::info() << endl;

	int rc = system(c.c_str());

	if (assertSuccess && rc != 0)
	{
		throw SeriousBug(string(" \"") + cmd + "\" failed. " + file + " +" + Translator<int, string>()(line));
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

std::string StringTool::upper(const string& v)
{
	string r = v;
	transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (toupper));
	return r;
}

std::string StringTool::lower(const string& v)
{
	string r = v;
	transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (tolower));
	return r;
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

std::string StringTool::trim(const std::string &str)
{
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");

    if((string::npos == startpos) || (string::npos == endpos))
        return "";
    else
        return str.substr(startpos, endpos - startpos + 1);
}

void StringTool::trimInPlace(std::string &str) { str = trim(str); }

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

vector<std::string> StringTool::split(const string &delim, const string &text)
{
	vector<std::string> ss;
	Tokenizer tokenizer(delim);
	tokenizer(text, ss);
	return ss;
}

std::string StringTool::join(const string &delimiter, const vector<std::string>& words)
{
	string r;
	for (size_t i = 0; i < words.size(); ++i)
	{
		if (i > 0)
			r += delimiter;
		r += words[i];
	}
	return r;
}

ostream& operator<<(ostream& s, const vector<string>& st) 
{
    s << '[';
    for (vector<string>::const_iterator it = st.begin(); it != st.end(); ++it)
        s << *it << ",";
    s << ']';
    return s;
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
} // namespace odb 

