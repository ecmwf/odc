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

class CompactTool : public Tool {
public:

    CompactTool(int argc, char* argv[]);

    void run() override;

    static void help(std::ostream& o) { o << "Tries to compress a file"; }

    static void usage(const std::string& name, std::ostream& o) { o << name << " <input.odb> <output.odb>"; }

private:

    // No copy allowed

    CompactTool(const CompactTool&);
    CompactTool& operator=(const CompactTool&);
};

template <>
struct ExperimentalTool<CompactTool> {
    enum {
        experimental = true
    };
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc::tool
