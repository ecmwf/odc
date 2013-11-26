/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/StringTools.h"

#include "odblib/odb_api.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/ImportTool.h"
#include "odblib/SQLSelectFactory.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {

string ImportTool::defaultDelimiter_ = ",";

ImportTool::ImportTool(int argc, char *parameters[])
: Tool(argc, parameters)
{
	registerOptionWithArgument("-d"); // Delimiter
	registerOptionWithArgument("-sql"); // SQL to filter input CSV with
}

void ImportTool::run()
{
	if (parameters().size() != 3)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

	PathName inFile = parameters(1);
	PathName outFile = parameters(2);

	Log::info() << "ImportTool::run: inFile: " << inFile << ", outFile: " << outFile << std::endl;

	string delimiter = StringTools::upper(optionArgument("-d", defaultDelimiter()));
	if (delimiter == "TAB")
		delimiter = "\t";

    std::string sql(optionArgument("-sql", std::string("select *;")));
	
	filterAndImportFile(inFile, outFile, sql, delimiter);
}

void ImportTool::importFile(const PathName& in, const PathName& out, const string& delimiter)
{
    filterAndImportFile(in, out, "select *;", delimiter);
}

void ImportTool::filterAndImportFile(const PathName& in, const PathName& out, const string& sql, const string& delimiter)
{
	odb::sql::SQLSelectFactory::instance().csvDelimiter(delimiter);

	ifstream fs( in.asString().c_str() );
	odb::Select input(sql, fs, delimiter);

	odb::Writer<> writer(out);
	odb::Writer<>::iterator output(writer.begin());
	unsigned long long n = output->pass1(input.begin(), input.end());

    Log::info() << "ImportTool::importFile: Copied " << n << " rows." << std::endl;
}

void ImportTool::importText(const string& s, const PathName& out, const string& delimiter)
{
	odb::sql::SQLSelectFactory::instance().csvDelimiter(delimiter);

	stringstream fs(s);
	odb::Select input("select *;", fs, delimiter);

	odb::Writer<> writer(out);
	odb::Writer<>::iterator output(writer.begin());

	unsigned long long n = output->pass1(input.begin(), input.end());

    Log::info() << "ImportTool::importText: Copied " << n << " rows." << std::endl;
}

} // namespace tool 
} // namespace odb 

