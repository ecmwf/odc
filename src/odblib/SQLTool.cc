/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/PartFileHandle.h"
#include "odblib/odb_api.h"
#include "odblib/ToolFactory.h"
#include "odblib/SQLSelectFactory.h"
#include "odblib/SQLTool.h"
#include "odblib/Tool.h"

using namespace eclib;
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
	delimiter_ = optionArgument("-delimiter", string("\t"));

	if ((inputFile_ = optionArgument("-i", string(""))) == "-")
		inputFile_ = "/dev/tty";

	if ((outputFile_ = optionArgument("-o", string(""))) == "-")
		outputFile_ = "/dev/tty";

	outputFormat_ = optionArgument("-f", string("default"));

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
		Log::error() << endl;
		return;// 1;
	}
	vector<string> params(parameters());
	params.erase(params.begin());

	auto_ptr<ofstream> foutPtr(optionIsSet("-o")
								? new ofstream(optionArgument("-o", string("")).c_str())
								: 0);
	ostream& out(foutPtr.get() ? *foutPtr : cout);
	SQLInteractiveSession session(out);
	SQLParser parser;
	SQLOutputConfig config(SQLSelectFactory::instance().config());
	string sql(StringTool::match("select", params[0])
				? StringTools::join(" ",  params) + ";"
				: StringTool::readFile(params[0] == "-" ? "/dev/tty" : params[0]));
	runSQL(sql, session, parser, config);
}

void SQLTool::runSQL(const string& sql, SQLSession& session, SQLParser& parser, const SQLOutputConfig& config)
{
	Log::info() << "Executing '" << sql << "'" << endl;

	if (inputFile_.size() == 0)
		parser.parseString(sql, static_cast<DataHandle*>(0), config);
	else if (offset_ != Offset(0) || length_ != Length(0))
	{
		// FIXME: PartFileHandle doesn't think length 0 means to the end of the file - get correct length
		Log::info() << "Selecting " << length_ << " bytes from offset " << offset_ 
				<< " of " << inputFile_ << endl;
		PartFileHandle fh(inputFile_, offset_, length_); 
		fh.openForRead();
		parser.parseString(sql, &fh, config);
	} else {
		FileHandle fh(inputFile_);
		fh.openForRead();
		parser.parseString(sql, &fh, config);
	}
}

} // namespace tool 
} // namespace odb 

