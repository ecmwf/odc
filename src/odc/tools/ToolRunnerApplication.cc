/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ToolRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

#include "odc/tools/Tool.h"
#include "odc/tools/ToolFactory.h"
#include "odc/tools/ToolRunnerApplication.h"

using namespace eckit;

namespace odc {
namespace tool {

ToolRunnerApplication::ToolRunnerApplication(int argc, char** argv, bool createCommandLineTool, bool deleteTool) :
    ODBApplication(argc, argv),
    tool_(!createCommandLineTool ? 0
                                 : AbstractToolFactory::createTool(PathName(argv[1]).baseName(), argc - 1, argv + 1)),
    deleteTool_(deleteTool) {}

ToolRunnerApplication::ToolRunnerApplication(odc::tool::Tool& tool, int argc, char** argv) :
    ODBApplication(argc, argv), tool_(&tool), deleteTool_(false) {}

ToolRunnerApplication::~ToolRunnerApplication() {
    if (deleteTool_)
        delete tool_;
}

void ToolRunnerApplication::tool(odc::tool::Tool* tool) {
    tool_ = tool;
}

void ToolRunnerApplication::run() {
    if (tool_ == 0) {
        std::cerr << name() << ": Unknown command '" << argv(1) << "'" << std::endl;
        throw eckit::UserError(std::string("Unknown command: ") + argv(1), Here());
    }

    tool_->run();

    if (deleteTool_) {
        delete tool_;
        tool_ = 0;
    }
}


int ToolRunnerApplication::printHelp(std::ostream& out) {
    if (tool_ == 0) {
        std::cerr << name() << ": Unknown command '" << argv(1) << "'" << std::endl;
        return 1;
    }
    // tool_->help(out);
    return 0;
}

}  // namespace tool
}  // namespace odc
