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

#include "tools/CompactTool.h"
#include "tools/CompareTool.h"
#include "tools/CountTool.h"
#include "tools/FixedSizeRowTool.h"
#include "tools/ImportTool.h"
#include "tools/LSTool.h"
#include "tools/MDSetTool.h"
#include "tools/MergeTool.h"
#include "tools/ODA2RequestTool.h"
#include "tools/ODAHeaderTool.h"
#include "tools/SQLTool.h"
#include "tools/SetTool.h"
#include "tools/SplitTool.h"
#include "tools/Tool.h"
#include "tools/ToolFactory.h"
#include "tools/XYVTool.h"

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

