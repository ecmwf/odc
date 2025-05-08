/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#ifndef MDSetTool_H
#define MDSetTool_H

#include "eckit/sql/SQLTypedefs.h"
#include "odc/tools/Tool.h"

namespace odc {
namespace tool {

class MDSetTool : public Tool {
public:

    MDSetTool(int argc, char* argv[]);

    void run();

    static void help(std::ostream& o);

    static void usage(const std::string& name, std::ostream& o);

private:

    // No copy allowed

    MDSetTool(const MDSetTool&);
    MDSetTool& operator=(const MDSetTool&);

    void parseUpdateList(const std::string& s, std::vector<std::string>& columns, std::vector<std::string>& types,
                         std::vector<std::string>& values, std::vector<eckit::sql::BitfieldDef>& bitfieldDefs);
};

}  // namespace tool
}  // namespace odc

#endif
