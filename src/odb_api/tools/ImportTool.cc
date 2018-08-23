/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"
#include "eckit/parser/StringTools.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"

#include "odb_api/csv/TextReader.h"
#include "odb_api/csv/TextReaderIterator.h"
#include "odb_api/sql/SQLOutputConfig.h"
#include "odb_api/Select.h"
#include "odb_api/SelectIterator.h"
#include "odb_api/TODATable.h"
#include "odb_api/tools/ImportTool.h"
#include "odb_api/Writer.h"


using namespace std;
using namespace eckit;

namespace odb {
namespace tool {

// TODO: A test with wide strings
// TODO: A test with SQL filtering
// TODO: A test with non-comma delimiters

ImportTool::ImportTool(int argc, char *parameters[])
: Tool(argc, parameters)
{
	registerOptionWithArgument("-d"); // Delimiter
	registerOptionWithArgument("-sql"); // SQL to filter input CSV with
}

void ImportTool::help(std::ostream &o) {
    o << "Imports data from a text file";
}

void ImportTool::usage(const std::string& name, std::ostream &o) {
    o << name
      << "\t[-d delimiter] <input.file> <output.file>" << std::endl
      << std::endl
      << "\tdelimiter can be a single character (e.g.: ',') or TAB. As a data example:" << std::endl
      << std::endl
      << "\tcol1:INTEGER,col2:REAL" << std::endl
      << "\t1,2.0" << std::endl
      << "\t3,4.0" << std::endl;
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

    PathName inFile (parameters(1)),
             outFile (parameters(2));

    Log::info() << "ImportTool::run: inFile: " << inFile << ", outFile: " << outFile << std::endl;

    std::string delimiter (StringTools::upper(optionArgument("-d", defaultDelimiter())));
    delimiter = delimiter == "TAB" ? "\t" 
              : delimiter == "SPACE" ? " "
              : delimiter;

    std::string sql (optionArgument("-sql", std::string("select *;")));

    filterAndImportFile (inFile, outFile, sql, delimiter);
}

void ImportTool::importFile(const PathName& in, const PathName& out, const std::string& delimiter)
{
    filterAndImportFile(in, out, "select *;", delimiter);
}

void ImportTool::filterAndImportFile(const PathName& in, const PathName& out, const std::string& sql, const std::string& delimiter)
{
    // TODO: Why are we not using the ODAOutput directly, rather than going via a Select, Writer combination?

    eckit::sql::SQLSession session(std::unique_ptr<odb::sql::SQLOutputConfig>(new odb::sql::SQLOutputConfig(out)));

    eckit::sql::SQLDatabase& db(session.currentDatabase());
    db.addImplicitTable(new odb::sql::ODBCSVTable(db, in, in, delimiter));
    session.currentDatabase();

    eckit::sql::SQLParser().parseString(session, sql);
    size_t n = session.statement().execute();

    Log::info() << "ImportTool::importFile: Copied " << n << " rows." << std::endl;
}

void ImportTool::importText(const std::string& s, const PathName& out, const std::string& delimiter)
{
    // TODO: There is no reason to be doing an SQL select * here! Just parse the damn file!

    std::stringstream ss(s);
    odb::TextReader reader(ss, delimiter);

	odb::Writer<> writer(out);
	odb::Writer<>::iterator output(writer.begin());

    unsigned long long n = output->pass1(reader.begin(), reader.end());

    Log::info() << "ImportTool::importText: Copied " << n << " rows." << std::endl;
}

} // namespace tool 
} // namespace odb 

