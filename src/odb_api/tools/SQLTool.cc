/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/FileHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/FileDescHandle.h"
#include "eckit/parser/StringTools.h"

#include "odb_api/SQLInteractiveSession.h"
#include "odb_api/SQLParser.h"
#include "odb_api/odb_api.h"
#include "odb_api/SQLSelectFactory.h"
#include "odb_api/tools/SQLTool.h"

using namespace std;
using namespace eckit;
using namespace odb::sql;

namespace odb {
namespace tool {

SQLTool::SQLTool(int argc,char **argv)
: Tool(argc,argv),
  sqlOutputConfig_(),
  inputFile_(),
  offset_(),
  length_()
{
	registerOptionWithArgument("-o");
	registerOptionWithArgument("-i");
	registerOptionWithArgument("-I");
	registerOptionWithArgument("-delimiter");
	registerOptionWithArgument("-f"); // output format 
	registerOptionWithArgument("-offset"); 
	registerOptionWithArgument("-length");

	sqlOutputConfig_.doNotWriteColumnNames(optionIsSet("-T"));
	sqlOutputConfig_.doNotWriteNULL(optionIsSet("-N"));
	sqlOutputConfig_.fieldDelimiter(optionArgument("-delimiter", std::string("\t")));

    if ((inputFile_ = optionArgument("-i", std::string(""))) == "-")
		inputFile_ = "/dev/stdin";

    sqlOutputConfig_.outputFile(optionArgument("-o", std::string("")));
    if (sqlOutputConfig_.outputFile() == "-")
		sqlOutputConfig_.outputFile("/dev/stdout");

    sqlOutputConfig_.outputFormat(optionArgument("-f", std::string("default")));
    sqlOutputConfig_.displayBitfieldsBinary(optionIsSet("--bin") || optionIsSet("--binary"));
    sqlOutputConfig_.displayBitfieldsHexadecimal(optionIsSet("--hex") || optionIsSet("--hexadecimal"));
    sqlOutputConfig_.disableAlignmentOfColumns(optionIsSet("--no_alignment"));

	offset_ = optionArgument("-offset", (long) 0); // FIXME@ optionArgument should accept unsigned long etc
	length_ = optionArgument("-length", (long) 0);
}

SQLTool::~SQLTool() {}

void SQLTool::run()
{
    if (parameters().size() < 2)
    {
        Log::error() << "Usage: ";
        usage(parameters(0), Log::error());
        Log::error() << std::endl;
        return;// 1;
    }
    std::vector<std::string> params(parameters());
    params.erase(params.begin());

    std::string sql(StringTool::isSelectStatement(params[0])
                ? StringTools::join(" ",  params) + ";"
                // FIXME:
                : StringTool::readFile(params[0] == "-" ? "/dev/tty" : params[0]));
    std::auto_ptr<std::ofstream> foutPtr(optionIsSet("-o")
                                ? new std::ofstream(optionArgument("-o", std::string("")).c_str())
                                : 0);
    std::ostream& out(foutPtr.get() ? *foutPtr : std::cout);
    SQLInteractiveSession session(out);
    session.selectFactory().config(sqlOutputConfig_);
    SQLOutputConfig config(session.selectFactory().config());
    PathName inputFile(inputFile_);
    SQLParser parser;
    runSQL(sql, inputFile, session, parser, config, offset_, length_);
}

void SQLTool::execute(const string& sql)
{
    execute(sql, cout);
}

void SQLTool::execute(const string& sql, ostream& out)
{
    SQLInteractiveSession session(out);
    SQLParser parser;
    SQLOutputConfig config(session.selectFactory().config());
    runSQL(sql, "", session, parser, config);
}

void SQLTool::runSQL(const string& sql, const PathName& inputFile, SQLSession& session, SQLParser& parser, const SQLOutputConfig& config)
{
    if (inputFile.path().size() == eckit::Length(0)) {
        parser.parseString(session, sql, static_cast<DataHandle*>(0), config);
    } else if (inputFile == "/dev/stdin" || inputFile == "stdin") {
        Log::info() << "Reading from standard input" << std::endl;
        FileDescHandle fh(0);
        fh.openForRead();
        parser.parseString(session, sql, &fh, config);
    } else {
        FileHandle fh(inputFile);
        fh.openForRead();
        parser.parseString(session, sql, &fh, config);
    }
}

void SQLTool::runSQL(const string& sql, const PathName& inputFile, SQLSession& session, SQLParser& parser, const SQLOutputConfig& config, const Offset& offset, const Length& length)
{
    if (offset == Offset(0) && length == Length(0))
    {
        runSQL(sql, inputFile, session, parser, config);
        return;
    }

    if (inputFile.path().size() == eckit::Length(0))
        parser.parseString(session, sql, static_cast<DataHandle*>(0), config);
    else
    {
        Log::info() << "Selecting " << length << " bytes from offset " << offset << " of " << inputFile << std::endl;
        PartFileHandle fh(inputFile, offset, length); 
        fh.openForRead();
        parser.parseString(session, sql, &fh, config);
    } 
}


} // namespace tool 
} // namespace odb 

