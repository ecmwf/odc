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
#include "Comparator.h"
#include "CompareTool.h"
#include "StringTool.h"



namespace odb {
namespace tool {

ToolFactory<CompareTool> compare("compare");

CompareTool::CompareTool (int argc, char *argv[])
: Tool(argc, argv) 
{
	registerOptionWithArgument("-excludeColumnsTypes");
	if (parameters().size() != 3)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	ASSERT("First file does not exist!" && PathName(parameters()[1]).exists());
	ASSERT("Second file does not exist!" && PathName(parameters()[2]).exists());

	//ASSERT("First parameter is a directory name!" && !path1.isDir());
	//ASSERT("Second parameter is a directory name!" && !path2.isDir());

	file1 = new PathName(parameters()[1]);
	file2 = new PathName(parameters()[2]);
}


void CompareTool::run()
{
	Timer t(string("Comparing files ") + *file1 + " and " + *file2);
	odb::Reader oda1(*file1);
	odb::Reader oda2(*file2);

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());

	vector<string> excludedColumnsTypes = StringTools::split(",", optionArgument("-excludeColumnsTypes", string("")));

	Log::info() << "excludedColumnsTypes:" << excludedColumnsTypes << endl;
	
	bool checkMissing = ! optionIsSet("-dontCheckMissing");
	odb::Comparator(checkMissing).compare(it1, end1, it2, end2, *file1, *file2, excludedColumnsTypes);
}

} // namespace tool 
} // namespace odb 

