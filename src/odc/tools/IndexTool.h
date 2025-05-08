/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta, ECMWF, Oct 2015

#ifndef IndexTool_H
#define IndexTool_H

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#include "odc/tools/Tool.h"

#include <utility>
#include <vector>

namespace odc {
namespace tool {

typedef std::vector<std::pair<eckit::Offset, eckit::Length> > BlockOffsets;
typedef unsigned long long ullong;

class IndexTool : public Tool {
public:

    IndexTool(int argc, char* argv[]);

    void run();

    static void help(std::ostream& o);

    static void usage(const std::string& name, std::ostream& o);

private:

    // No copy allowed
    IndexTool(const IndexTool&);
    IndexTool& operator=(const IndexTool&);
};

}  // namespace tool
}  // namespace odc

#endif
