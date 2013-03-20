/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file Tool.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <strings.h>
#include <iostream>

using namespace std;

#include "eclib/runtime/Application.h"

#include "odblib/CommandLineParser.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"

#include "odblib/CompactTool.h"
#include "odblib/CompareTool.h"
#include "odblib/CountTool.h"
#include "odblib/FixedSizeRowTool.h"
#include "odblib/ImportTool.h"
#include "odblib/LSTool.h"
#include "odblib/MDSetTool.h"
#include "odblib/MergeTool.h"
#include "odblib/ODA2RequestTool.h"
#include "odblib/ODAHeaderTool.h"
#include "odblib/SQLTool.h"
#include "odblib/SetTool.h"
#include "odblib/SplitTool.h"
#include "odblib/XYVTool.h"

namespace odb {
namespace tool {

Tool::~Tool() {}

Tool::Tool(int argc, char **argv)
: CommandLineParser(argc, argv)
{}

Tool::Tool(const CommandLineParser& clp)
: CommandLineParser(clp)
{}

void Tool::registerTools()
{
	static ToolFactory<CompactTool> compact("compact");
	static ToolFactory<CompareTool> compare("compare");
	static ToolFactory<CountTool> countTool("count");
	static ToolFactory<FixedSizeRowTool> fixedSizeRow("fixrowsize");
	static ToolFactory<ImportTool> import("import");
	static ToolFactory<LSTool> lsTool("ls");
	static ToolFactory<MDSetTool> mdset("mdset");
	static ToolFactory<MergeTool> mergeTool("merge");
	static ToolFactory<ODA2RequestTool> oda2requestTool("oda2request");
	static ToolFactory<HeaderTool> odaHeader("header");
	static ToolFactory<SQLTool> sqlTool("sql");
	static ToolFactory<SetTool> set("set");
	static ToolFactory<SplitTool> split("split");
	static ToolFactory<XYVTool> xyvTool("xyv");	
}

} // namespace tool 
} // namespace odb 

