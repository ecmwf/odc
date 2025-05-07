/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ImportTool_H
#define ImportTool_H

#include "eckit/filesystem/PathName.h"

#include "odc/tools/Tool.h"

namespace odc {
namespace tool {

class ImportTool : public Tool {
public:

    ImportTool(int argc, char* argv[]);

    void run();

    static void help(std::ostream& o);
    static void usage(const std::string& name, std::ostream& o);

    static void importFile(const eckit::PathName& in, const eckit::PathName& out,
                           const std::string& delimiter = defaultDelimiter());
    static void filterAndImportFile(const eckit::PathName& in, const eckit::PathName& out, const std::string& sql,
                                    const std::string& delimiter = defaultDelimiter());

    static std::string defaultDelimiter() { return ","; }

private:

    // No copy allowed
    ImportTool(const ImportTool&);
    ImportTool& operator=(const ImportTool&);
};

}  // namespace tool
}  // namespace odc

#endif
