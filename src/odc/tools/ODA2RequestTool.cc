/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/parser/StringTools.h"
#include "eckit/parser/Tokenizer.h"
#include "odc/FastODA2Request.h"
#include "odc/GribCodes.h"
#include "odc/Select.h"
#include "odc/tools/ODA2RequestTool.h"

using namespace std;
using namespace eckit;

namespace odc {
namespace tool {

char * static_argv[] = { const_cast<char *>("oda2request") };

ODA2RequestTool::ODA2RequestTool(int argc, char **argv)
: Tool(argc, argv)
{
	registerOptionWithArgument("-c");
	readConfig();
}

ODA2RequestTool::ODA2RequestTool()
: Tool(1, static_argv)
{
	registerOptionWithArgument("-c");
	readConfig();
}

ODA2RequestTool::~ODA2RequestTool() {}

void ODA2RequestTool::help(std::ostream &o) { o << "Creates MARS ARCHIVE request for a given file"; }

void ODA2RequestTool::usage(const std::string& name, std::ostream &o)
{
	o << name << " [-c configFile] [-q] <input-file.odb> [<output-file>]";
}

void ODA2RequestTool::run()
{
	eckit::PathName inputFile;
	string outputFile;

	switch (parameters().size())
	{
		case 3:
			outputFile = parameters(2);
		case 2:
			inputFile = parameters(1);
			break;
		default:
			Log::error() << "Usage: ";
			usage(parameters(0), Log::error());
			Log::error() << std::endl;
			return;// 1;
			break;
	}

	readConfig();

	string request = generateMarsRequest(inputFile, optionIsSet("-q"));

	if (outputFile.size() == 0)
		std::cout << request << std::endl;
	else
	{
		ofstream out(outputFile.c_str());
		out << request << std::endl;
		out.close();
	}
	
	return;
}

PathName ODA2RequestTool::config()
{
    return optionArgument("-c", std::string("~odc/codes/ODA2RequestTool.cfg"));
}

void ODA2RequestTool::readConfig() { readConfig(config()); }

void ODA2RequestTool::readConfig(const PathName& fileName)
{
	Log::debug() << "ODA2RequestTool::readConfig: reading file '" << fileName << "'" << std::endl;
	columnName2requestKey_.clear();

	string s = readFile(fileName);
	
	Log::debug() << "ODA2RequestTool::readConfig: parsing '" << s << "'" << std::endl;

	parseConfig(s);
}

void ODA2RequestTool::parseConfig(const std::string& s)
{
	Log::debug() << "ODA2RequestTool::parseConfig: '" << s << "'" << std::endl;

    vector<std::string> lines;
    Tokenizer("\n")(s, lines);

    Tokenizer tokenizer(": \t");
    for (size_t i = 0; i < lines.size(); ++i)
	{
		vector<std::string> words;
		tokenizer(lines[i], words);

		if (words.size() == 0)
			continue;

		ASSERT("Each line of config file should be like: 'MARS_KEYWORD : oda_column_name'" && words.size() == 2);
		columnName2requestKey_[words[1]] = words[0];
	}
}

inline string int_as_double2string(double v)
{
	stringstream s;
	s.precision(0);
	s << fixed << v;
	return s.str();
}

string ODA2RequestTool::gatherStatsFast(const PathName& inputFile)
{
	FastODA2Request<ODA2RequestClientTraits> o;
	o.parseConfig(readFile(config()));
	o.scanFile(inputFile);
	return o.genRequest();
}

void ODA2RequestTool::gatherStats(const PathName& inputFile)
{
	size_t n = columnName2requestKey_.size();
	values_ = vector<Values>(n);

	string columnList;
	for (std::map<string, string>::iterator it = columnName2requestKey_.begin();
		 it != columnName2requestKey_.end();
		 ++it)
	{
		if (it != columnName2requestKey_.begin())
			columnList += ", ";
		columnList += it->first;
	}
	
	const string select = std::string("select ") + columnList + " from \"" + inputFile + "\";";
	Log::info() << "Executing '" << select << "'" << std::endl;

	Translator<double, string> double2string;
	odc::Select oda(select, inputFile);
	odc::Select::iterator end = oda.end();
	for (odc::Select::iterator row = oda.begin(); row != end; ++row) 
		for (size_t i = 0; i < n; ++i)
		{
			odc::ColumnType type = row->columns()[i]->type();
			Value v = type == odc::STRING ? (*row).string(i)
					: type == odc::INTEGER ? int_as_double2string((*row)[i])
					: double2string((*row)[i]);
			values_[i].insert(v);
		}
}

string ODA2RequestTool::generateMarsRequest(const PathName& inputFile, bool fast)
{
	stringstream request;

	if (fast)
		request << gatherStatsFast(inputFile);
	else
	{
		gatherStats(inputFile);

		size_t i = 0;
		std::map<string, string>::iterator end = columnName2requestKey_.end();
		for (std::map<string, string>::iterator it = columnName2requestKey_.begin(); it != end; ++it)
		{
			if (request.str().size()) request << ",\n";

			const std::string& key = it->second;
			const string k = StringTools::upper(key);

			string valuesList;	
			Values& vs = values_[i++];
			for (Values::iterator vi = vs.begin(); vi != vs.end(); ++vi)
			{
				string v = *vi;
				Log::debug() << "ODA2RequestTool::genRequest: v = '" << v  << "', key = " << key << std::endl;
				if (k == "TIME")
					v = StringTool::patchTimeForMars(v);
				else
				if (k == "CLASS" || k == "TYPE" || k == "STREAM")
				{
					Log::debug() << "ODA2RequestTool::genRequest: checking if '" << v << "' is numeric" << std::endl;
					if (StringTool::check(v, isdigit))
					{
						v = StringTools::trim(v);
						Log::debug() << "ODA2RequestTool::genRequest: replacing " << v << " with ";
						v = GribCodes::alphanumeric(StringTools::lower(key), v);
						Log::debug() << v << std::endl;
					}
					v = StringTools::upper(v);
				}

				if (vi != vs.begin())
					valuesList += "/";

				valuesList += v;
			}
			request << key << " = " << valuesList;
		}
	}

	stringstream str;
	str << "ODB," << std::endl;
	str << request.str();
	return str.str();
}

} // namespace tool 
} // namespace odc 

