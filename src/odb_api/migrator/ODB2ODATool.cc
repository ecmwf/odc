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

#include "eckit/parser/StringTools.h"
#include "odb_api/migrator/FakeODBIterator.h"
#include "odb_api/migrator/ImportODBTool.h"
#include "odb_api/migrator/ODB2ODATool.h"
#include "odb_api/migrator/ODBIterator.h"
#include "odb_api/migrator/OldODBReader.h"
#include "odb_api/migrator/ReptypeGenIterator.h"
#include "odb_api/MDI.h"
#include "odb_api/odb_api.h"
#include "odb_api/tools/Tool.h"
#include "odb_api/tools/ToolFactory.h"


using namespace eckit;

namespace odb {
namespace tool {

ODB2ODATool::ODB2ODATool (int argc, char *argv[])
: Tool(argc, argv)
{}

ODB2ODATool::ODB2ODATool (const CommandLineParser &clp)
: Tool(clp)
{}


/// -mdi <type1:MDI1,type2:MDI2,...>
void ODB2ODATool::resetMDI(const std::string& s)
{
    typedef eckit::StringTools S;
    std::vector<std::string> columns(S::split(",", s));
    for (size_t i = 0; i < columns.size(); ++i)
    {
        std::vector<std::string> ass(S::split(":", columns[i]));

        if (ass.size() != 2)
            throw UserError("Error parsing option -mdi");

        const std::string typeName(S::upper(ass[0]));
        double value(StringTool::translate(ass[1]));

        Log::info() << "  typeName: " << typeName << " value: " << value << std::endl;

        if (typeName == "REAL")
            odb::MDI::realMDI(value);
        else if (typeName == "INTEGER" || typeName == "INT")
            odb::MDI::integerMDI(value);
        else
            throw UserError("Changing MDI of types different than INTEGER or REAL not supported yet.");
    }
}

void ODB2ODATool::run()
{
	if (parameters().size() < 2 || parameters().size() > 4)
	{
        std::cerr << "Usage:" << std::endl
			<< "	" << parameters(0)

            << " [<options>] <odb_database> [<file-with-select-statement-defining-dump> [<output.odb>]]" << std::endl

            << "Options: " << std::endl << std::endl

            << "\t[-genreptype <list-of-columns>]" << std::endl
            << "\t[-reptypecfg <reptype-generation-config-file>]" << std::endl
            << "\t[-addcolumns <list-of-assignments>]" << std::endl
            << "\t[-mdi <type1:MDI1,type2:MDI2,...>]              Provide values of missing data indicators, e.g.: -mdi REAL:2147483647,INTEGER:2147483647" << std::endl
            << "\t[-no_verification]                              Do not verify the conversion" << std::endl

			<< std::endl;
		return;
	}

    std::string nonDefaultMDIS(optionArgument<std::string>("-mdi", ""));
    if (nonDefaultMDIS.size())
        Log::info() << "Using non default missing data indicators: " << nonDefaultMDIS << std::endl;

	bool addColumns (optionIsSet("-addcolumns"));
	if (addColumns)
		FakeODBIterator::ConstParameters::instance().add(Assignments(optionArgument<std::string>("-addcolumns", "")));
 
	bool genReptype (optionIsSet("-genreptype"));
	bool reptypeCfg (optionIsSet("-reptypecfg"));
    if (optionIsSet("-mdi"))
        resetMDI(optionArgument<std::string>("-mdi", ""));

	ASSERT("Only one of -genreptype and -reptypecfg can be choosen at a time." && !(genReptype && reptypeCfg));

	if (genReptype) {
        std::vector<std::string> columns = StringTools::split(",", optionArgument<std::string>("-genreptype", ""));
		ReptypeTableConfig::addColumns(columns.begin(), columns.end());
	}

	if (reptypeCfg) ReptypeTableConfig::load(optionArgument<std::string>("-reptypecfg", ""));

	if (addColumns && (genReptype || reptypeCfg)) {
		typedef odb::tool::TSQLReader<ReptypeGenIterator<FakeODBIterator> > R;
		ImportODBTool<R>(*this).run();
		return;
	}

	if (addColumns) {
		ImportODBTool<odb::tool::TSQLReader<FakeODBIterator> >(*this).run();
		return;
	}

	if (genReptype || reptypeCfg) {
		ImportODBTool<odb::tool::TSQLReader<ReptypeGenIterator<ODBIterator> > >(*this).run();
		return;
	}

	{	
		ImportODBTool<odb::tool::OldODBReader>(*this).run();
		Log::info() << "ImportODBTool<ODBIterator> finished OK" << std::endl;
	}
}

} // namespace tool 
} //namespace odb 

