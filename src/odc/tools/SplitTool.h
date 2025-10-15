/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SplitTool_H
#define SplitTool_H

#include <ostream>
#include <string>
#include <vector>

#include "Tool.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

namespace odc {
namespace tool {

class SplitTool : public Tool {
public:

    SplitTool(int argc, char* argv[]);

    void run();

    static void help(std::ostream& o) { o << "Splits file according to given template"; }

    static void usage(const std::string& name, std::ostream& o) {
        o << name << " [-no_verification] [-maxopenfiles <N>] <input.odb> <output_template.odb>";
    }

    static void split(const eckit::PathName&, const std::string&, size_t, bool verify = true);
    static void presortAndSplit(const eckit::PathName&, const std::string&);

    static std::vector<std::pair<eckit::Offset, eckit::Length> > getChunks(const eckit::PathName&,
                                                                           size_t maxExpandedSize = 100 * 1024 * 1024);

private:

    // No copy allowed
    SplitTool(const SplitTool&);
    SplitTool& operator=(const SplitTool&);

    static std::string genOrderBySelect(const std::string&, const std::string&);

    long maxOpenFiles_;
    bool sort_;
};

}  // namespace tool
}  // namespace odc

#endif
