/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ODB2ODATool.cc
///
/// @author Piotr Kuchta, ECMWF, July 2009
///

#include <iostream>
#include <fstream>

#include "Tracer.h"

#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"

#include "ODBIterator.h"
#include "FakeODBIterator.h"
#include "ReptypeGenIterator.h"
#include "ImportODBTool.h"
#include "ODB2ODATool.h"
#include "OldODBReader.h"

namespace odb {
namespace tool {

ODB2ODATool::ODB2ODATool (int argc, char *argv[])
: Tool(argc, argv)
{}

ODB2ODATool::ODB2ODATool (const Application *app)
: Tool(app)
{}

ODB2ODATool::ODB2ODATool (const CommandLineParser &clp)
: Tool(clp)
{}

//ToolFactory<ODB2ODATool> odb2oda("odb2oda");

void ODB2ODATool::run()
{
	ASSERT ("Wrong number of parameters. Should be checked in odb2oda:main"
		&& !(parameters().size() < 2 || parameters().size() > 4));

	// TODO: the parameters -addcolumns and -genreptype should take arguments - list of columns to, respectively, be added or used to gen. reptype
	bool addColumns = optionIsSet("-addcolumns");
	if (addColumns)
		FakeODBIterator::ConstParameters::instance().add(Assignments(optionArgument<std::string>("-addcolumns", "")));
 
	bool genReptype = optionIsSet("-genreptype");
	bool reptypeCfg = optionIsSet("-reptypecfg");

	ASSERT("Only one of -genreptype and -reptypecfg can be choosen at a time." && !(genReptype && reptypeCfg));

	if (genReptype)
	{
		vector<std::string> columns = Tool::split(",", optionArgument<std::string>("-genreptype", ""));
		ReptypeTableConfig::addColumns(columns.begin(), columns.end());
	}

	if (reptypeCfg)
		ReptypeTableConfig::load(optionArgument<std::string>("-reptypecfg", ""));

	if (addColumns && (genReptype || reptypeCfg))
	{
		Tracer t(Log::info(), "odb2oda: Running ImportODBTool<ReptypeGenIterator<FakeODBIterator> >"); 

		typedef odb::tool::TSQLReader<ReptypeGenIterator<FakeODBIterator> > R;
		ImportODBTool<R>(*this).run();
		return; // 0;
	}

	if (addColumns)
	{
		Tracer t(Log::info(), "odb2oda: Running ImportODBTool<FakeODBIterator>"); 

		ImportODBTool<odb::tool::TSQLReader<FakeODBIterator> >(*this).run();
		return; //0;
	}

	if (genReptype || reptypeCfg)
	{
		Tracer t(Log::info(), "odb2oda: Running ImportODBTool<ReptypeGenIterator<ODBIterator> >"); 
		ImportODBTool<odb::tool::TSQLReader<ReptypeGenIterator<ODBIterator> > >(*this).run();
		return; // 0;
	}

	{	
		Tracer t(Log::info(), "odb2oda: Running ImportODBTool<ODBIterator>"); 
		ImportODBTool<odb::tool::OldODBReader>(*this).run();
		Log::info() << "ImportODBTool<ODBIterator> finished OK" << endl;
	}
	return;// 0;
}

} // namespace tool 
} //namespace odb 

