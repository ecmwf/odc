/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/io/PartFileHandle.h"

//#include "odblib/odb_api.h"
//#include "odblib/ToolFactory.h"
//#include "odblib/SQLSelectFactory.h"
#include "odblib/SQLTool.h"
//#include "odblib/Tool.h"

using namespace eckit;
using namespace odb::sql;

namespace odb {
namespace tool {

SQLTool::SQLTool(int argc,char **argv)
: Tool(argc,argv),
  offset_(),
  length_()
{
	registerOptionWithArgument("-o");
	registerOptionWithArgument("-i");
	registerOptionWithArgument("-delimiter");
	registerOptionWithArgument("-f"); // output format 
	registerOptionWithArgument("-offset"); 
	registerOptionWithArgument("-length");

	doNotWriteColumnNames_ = optionIsSet("-T");
	doNotWriteNULL_ = optionIsSet("-N");
	delimiter_ = optionArgument("-delimiter", std::string("\t"));

	if ((inputFile_ = optionArgument("-i", std::string(""))) == "-")
		inputFile_ = "/dev/tty";

	if ((outputFile_ = optionArgument("-o", std::string(""))) == "-")
		outputFile_ = "/dev/tty";

	outputFormat_ = optionArgument("-f", std::string("default"));

	offset_ = optionArgument("-offset", (long) 0); // FIXME@ optionArgument should accept unsigned long etc
	length_ = optionArgument("-length", (long) 0);

	SQLSelectFactory::instance()
		.config(SQLOutputConfig(doNotWriteColumnNames_, doNotWriteNULL_, delimiter_, outputFile_, outputFormat_));
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
				: StringTool::readFile(params[0] == "-" ? "/dev/tty" : params[0]));
	auto_ptr<ofstream> foutPtr(optionIsSet("-o")
								? new ofstream(optionArgument("-o", std::string("")).c_str())
								: 0);
	ostream& out(foutPtr.get() ? *foutPtr : cout);
	SQLInteractiveSession session(out);
	SQLParser parser;
	SQLOutputConfig config(SQLSelectFactory::instance().config());
    PathName inputFile(inputFile_);
	runSQL(sql, inputFile, session, parser, config, offset_, length_);
}

void SQLTool::execute(const std::string& sql, std::ostream& out)
{
	SQLInteractiveSession session(out);
	SQLParser parser;
	SQLOutputConfig config(SQLSelectFactory::instance().config());
	runSQL(sql, "", session, parser, config);
}

void SQLTool::runSQL(const std::string& sql, const eckit::PathName& inputFile, SQLSession& session, SQLParser& parser, const SQLOutputConfig& config, const Offset& offset, const Length& length)
{
	Log::info() << "Executing '" << sql << "'" << std::endl;

	if (inputFile.path().size() == eckit::Length(0))
		parser.parseString(sql, static_cast<DataHandle*>(0), config);
	else if (offset != Offset(0) || length != Length(0))
	{
		// FIXME: PartFileHandle doesn't think length 0 means to the end of the file - get correct length
		Log::info() << "Selecting " << length << " bytes from offset " << offset
				<< " of " << inputFile << std::endl;
		PartFileHandle fh(inputFile, offset, length); 
		fh.openForRead();
		parser.parseString(sql, &fh, config);
	} else {
		FileHandle fh(inputFile);
		fh.openForRead();
		parser.parseString(sql, &fh, config);
	}
}


} // namespace tool 
} // namespace odb 

