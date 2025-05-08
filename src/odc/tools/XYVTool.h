/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XYVTool_H
#define XYVTool_H

#include <ostream>

#include "Tool.h"

namespace odc {
namespace tool {

class XYVTool : public Tool {

public:

    XYVTool(int argc, char** argv);
    ~XYVTool();

    static void help(std::ostream& o) {
        o << "Creates XYV representation of file for displaying in a graphics program";
    }

    static void usage(const std::string& name, std::ostream& o) {
        o << name << " <input-file.odb> <value-column> <output-file.odb>";
    }

    virtual void run();
};

template <>
struct ExperimentalTool<XYVTool> {
    enum {
        experimental = true
    };
};

}  // namespace tool
}  // namespace odc

#endif
