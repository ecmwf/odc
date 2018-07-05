/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/parser/StringTools.h"
#include "eckit/log/Timer.h"
#include "odb_api/Comparator.h"
#include "odb_api/Reader.h"
#include "odb_api/tools/CompareTool.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {

CompareTool::CompareTool (int argc, char *argv[])
: Tool(argc, argv) 
{
	registerOptionWithArgument("-excludeColumnsTypes");
    registerOptionWithArgument("-excludeColumns");
    if (parameters().size() != 3)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		throw Exception("Wrong number of parameters.");
	}

	PathName p;
	if (! (p = PathName(parameters()[1])).exists()
		|| ! (p = PathName(parameters()[2])).exists())
	{
		stringstream s;
		s << "File " << p << " does not exist.";
		throw Exception(s.str());
	}

    file1_ = new PathName(parameters()[1]);
    file2_ = new PathName(parameters()[2]);
}


void CompareTool::run()
{
    Timer t(std::string("Comparing files ") + *file1_ + " and " + *file2_);
    odb::Reader oda1(*file1_);
    odb::Reader oda2(*file2_);

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());

	std::vector<std::string> excludedColumnsTypes = StringTools::split(",", optionArgument("-excludeColumnsTypes", std::string("")));
    std::vector<std::string> excludedColumns = StringTools::split(",", optionArgument("-excludeColumns", std::string("")));

    if (excludedColumnsTypes.size()) {
		Log::info() << "excludedColumnsTypes:" << excludedColumnsTypes << std::endl;
    }

    if (excludedColumns.size()) {
        Log::info() << "excludedColumns:" << excludedColumns << std::endl;
    }

	bool checkMissing = ! optionIsSet("-dontCheckMissing");
    odb::Comparator(checkMissing).compare(it1, end1, it2, end2, *file1_, *file2_, excludedColumnsTypes, excludedColumns);
}

} // namespace tool 
} // namespace odb 

