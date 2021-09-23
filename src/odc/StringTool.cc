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

#include "eckit/utils/StringTools.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Translator.h"
#include "eckit/utils/Regex.h"

#include "odc/LibOdc.h"
#include "odc/StringTool.h"

using namespace std;
using namespace eckit;

namespace odc {

std::vector<std::string> StringTool::readLines(const PathName fileName, bool logging)
{
	std::string s = readFile(fileName, logging);
	return StringTools::split("\n", s);
}

std::string StringTool::readFile(const PathName fileName, bool logging)
{
	const size_t CHUNK_SIZE = 1024;
	char buffer[CHUNK_SIZE]; 

	FileHandle f(fileName);
    f.openForRead();
    AutoClose close(f);
	
	std::string ret;
	size_t read, totalRead = 0;

	while ( (read = f.read(buffer, sizeof(buffer) / sizeof(char))) > 0 )
	{
		totalRead += read;
		ret.append(std::string(static_cast<char*>(buffer), read));
	}

	if (logging)
		Log::info()  << "Read " << totalRead << " bytes from file " << fileName << "[" << ret << "]" << std::endl;

	return ret;
}

int StringTool::shell(std::string cmd, const CodeLocation& where, bool assertSuccess)
{
	std::string c = "/bin/sh -c \"" + cmd + "\"";

	Log::info() << "Executing '" + c + "' ";
    Log::info() << " " << where.file() << " +" << where.line();
	Log::info() << std::endl;

	int rc = std::system(c.c_str());

	if (assertSuccess && rc != 0)
	{
		throw eckit::SeriousBug(std::string(" \"") + cmd + "\" failed. " + where.file() + " +" + Translator<int, std::string>()(where.line()));
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
    memset(buf, 0, sizeof(buf));
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

std::string StringTool::int_as_double2string(double v)
{
    std::stringstream s;
    s.precision(0);
    s << fixed << v;
    return s.str();
}

double StringTool::translate(const std::string& v)
{
	return isInQuotes(v) ? cast_as_double(unQuote(v)) : Translator<std::string, double>()(v);
}

void StringTool::trimInPlace(std::string &str) { str = StringTools::trim(str); }

bool StringTool::match(const std::string& regex, const std::string& s)
{
	return Regex(regex).match(s);
}

bool StringTool::matchAny(const std::vector<std::string>& regs, const std::string& s)
{
	for (size_t i = 0; i < regs.size(); ++i)
		if (match(regs[i], s))
			return true;
	return false;
}

ostream& operator<<(std::ostream& s, const std::vector<std::string>& st) 
{
    s << '[';
    for (std::vector<std::string>::const_iterator it = st.begin(); it != st.end(); ++it)
        s << *it << ",";
    s << ']';
    return s;
}


std::string StringTool::valueAsString(double d, api::ColumnType t)
{
    using namespace api;
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

std::string StringTool::patchTimeForMars(const std::string& ss)
{
	std::string v = ss;
	if (v.size() == 5) v = std::string("0") + v;
	if (v.size() == 6)
	{
		std::string s = v;
		v = v.substr(0, 4);
		LOG_DEBUG_LIB(odc::LibOdc)<< "StringTool::patchTimeForMars: stripping seconds from TIME: '"
				<< s << "' => '" << v << "'" << std::endl;
	}
	return v;
}

bool StringTool::isSelectStatement(const std::string& s) { return StringTool::match("select", eckit::StringTools::lower(eckit::StringTools::trim(s))); }

} // namespace odc 

