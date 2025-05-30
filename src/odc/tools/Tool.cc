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

#include "odc/tools/Tool.h"

#include <ostream>

#include "odc/tools/CompactTool.h"
#include "odc/tools/CompareTool.h"
#include "odc/tools/CountTool.h"
#include "odc/tools/FixedSizeRowTool.h"
#include "odc/tools/ImportTool.h"
#include "odc/tools/IndexTool.h"
#include "odc/tools/LSTool.h"
#include "odc/tools/MDSetTool.h"
#include "odc/tools/MergeTool.h"
#include "odc/tools/ODAHeaderTool.h"
#include "odc/tools/SQLTool.h"
#include "odc/tools/SetTool.h"
#include "odc/tools/SplitTool.h"
#include "odc/tools/ToolFactory.h"
#include "odc/tools/XYVTool.h"

namespace odc {
namespace tool {

Tool::~Tool() {}

Tool::Tool(int argc, char** argv) : CommandLineParser(argc, argv) {}

Tool::Tool(const CommandLineParser& clp) : CommandLineParser(clp) {}

void Tool::registerTools() {
    static ToolFactory<CompactTool> compact("compact");
    static ToolFactory<CompareTool> compare("compare");
    static ToolFactory<CountTool> countTool("count");
    static ToolFactory<IndexTool> indexTool("index");
    static ToolFactory<FixedSizeRowTool> fixedSizeRow("fixrowsize");
    static ToolFactory<ImportTool> import("import");
    static ToolFactory<LSTool> lsTool("ls");
    static ToolFactory<MDSetTool> mdset("mdset");
    static ToolFactory<MergeTool> mergeTool("merge");
    static ToolFactory<HeaderTool> odaHeader("header");
    static ToolFactory<SQLTool> sqlTool("sql");
    static ToolFactory<SetTool> set("set");
    static ToolFactory<SplitTool> split("split");
    static ToolFactory<XYVTool> xyvTool("xyv");
}

}  // namespace tool
}  // namespace odc
