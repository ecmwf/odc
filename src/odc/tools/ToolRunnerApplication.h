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

#ifndef ToolRunnerApplication_H
#define ToolRunnerApplication_H

#include "odc/ODBApplication.h"

namespace odc {
namespace tool {

class Tool;

class ToolRunnerApplication : public ODBApplication {
public:

    ToolRunnerApplication(int argc, char** argv, bool createCommandLineTool = true, bool deleteTool = true);
    ToolRunnerApplication(odc::tool::Tool& tool, int argc, char** argv);
    ~ToolRunnerApplication();

    void tool(odc::tool::Tool*);

    void run();
    int printHelp(std::ostream& out);

private:

    odc::tool::Tool* tool_;
    bool deleteTool_;
};

}  // namespace tool
}  // namespace odc

#endif
