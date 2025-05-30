/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include "odc/tools/Tool.h"

#include <ostream>

namespace odc::tool {

//----------------------------------------------------------------------------------------------------------------------

class FixedSizeRowTool : public Tool {
public:

    FixedSizeRowTool(int argc, char* argv[]);

    void run() override;

    static void help(std::ostream& o) { o << "Converts file to a format with fixed size rows"; }

    static void usage(const std::string& name, std::ostream& o) { o << name << " <input.odb> <output.odb>"; }

private:

    // No copy allowed

    FixedSizeRowTool(const FixedSizeRowTool&);
    FixedSizeRowTool& operator=(const FixedSizeRowTool&);
};

template <>
struct ExperimentalTool<FixedSizeRowTool> {
    enum {
        experimental = true
    };
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc::tool

