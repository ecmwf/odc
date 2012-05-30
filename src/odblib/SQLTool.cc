/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/oda.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "SQLTool.h"
#include "SQLOutputConfig.h"
#include "SQLSelectFactory.h"

namespace odb {
namespace tool {

ToolFactory<SQLTool> sqlTool("sql");

SQLTool::SQLTool(int argc,char **argv)
: Tool(argc,argv)
{
	registerOptionWithArgument("-o");
	registerOptionWithArgument("-i");
	registerOptionWithArgument("-delimiter");
	registerOptionWithArgument("-f"); // output format ( -f odb  for binary ODB format); default is ascii

	doNotWriteColumnNames_ = optionIsSet("-T");
	doNotWriteNULL_ = optionIsSet("-N");
	delimiter_ = optionArgument("-delimiter", string("\t"));

	inputFile_ = optionArgument("-i", string(""));
	if (inputFile_ == "-")
		inputFile_ = "/dev/tty";

	outputFile_ = optionArgument("-o", string(""));
	if (outputFile_ == "-")
		outputFile_ = "/dev/tty";

	outputFormat_ = optionArgument("-f", string("default"));

	odb::sql::SQLSelectFactory::instance()
		.config(odb::sql::SQLOutputConfig(doNotWriteColumnNames_, doNotWriteNULL_, delimiter_, outputFile_, outputFormat_));
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

	auto_ptr<ofstream> foutPtr;
	if (optionIsSet("-o") && outputFormat_ == "default") {
		foutPtr.reset(new ofstream(optionArgument("-o", string("")).c_str()));
    } 
	ostream& out = (optionIsSet("-o") && outputFormat_ == "default") ? *foutPtr : cout;

	if (string(parameters(1)) == "select" || StringTool::match("select", parameters(1)))
	{
		string sql;
		for (size_t i = 1; i < parameters().size(); i++)
			sql += parameters(i) + " ";
		sql += ";";
		Log::info() << "Executing '" << sql << "'" << endl;

		odb::sql::SQLInteractiveSession session(out);
		odb::sql::SQLParser p;
		if (inputFile_.size() == 0)
			p.parseString(sql, static_cast<DataHandle*>(0), odb::sql::SQLSelectFactory::instance().config());
		else
		{
			FileHandle fh(inputFile_);
			fh.openForRead();
			p.parseString(sql, &fh, odb::sql::SQLSelectFactory::instance().config());
		}
	}
	else
	{
		string fileName = parameters(1); //Resource<string>("-i", "test.sql");
		if (fileName == "-")
			fileName = "/dev/tty";

		Log::info() << "Executing SQL from " << fileName << endl;

		odb::sql::SQLInteractiveSession session(out);
		odb::sql::SQLParser p;
        
		if (inputFile_.size() == 0)
			p.parseString(StringTool::readFile(fileName), static_cast<DataHandle*>(0), odb::sql::SQLSelectFactory::instance().config());
		else
		{
			FileHandle fh(inputFile_);
			fh.openForRead();
			p.parseString(StringTool::readFile(fileName), &fh, odb::sql::SQLSelectFactory::instance().config());
		}
	}
}

} // namespace tool 
} // namespace odb 

