/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#ifndef HeaderTool_H
#define HeaderTool_H

#include "Tool.h"

namespace odc {
namespace tool {

class HeaderTool : public Tool {
public:

    HeaderTool(int argc, char* argv[]);

    void run();

    static void help(std::ostream& o) { o << "Shows header(s) and metadata(s) of file"; }

    static void usage(const std::string& name, std::ostream& o) {
        o << name << " [-offsets] [-ddl] [-table <table-name-in-the-generated-ddl>] <file-name>";
    }

private:

    // No copy allowed
    HeaderTool(const HeaderTool&);
    HeaderTool& operator=(const HeaderTool&);

    std::string readFile(const std::string& fileName);
};

}  // namespace tool
}  // namespace odc

#endif
