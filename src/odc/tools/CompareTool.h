/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef CompareTool_H
#define CompareTool_H

#include "odc/tools/Tool.h"

#include "eckit/filesystem/PathName.h"

namespace odc {

class RowsReaderIterator;

namespace tool {

class CompareTool : public Tool {
public:

    CompareTool(int argc, char* argv[]);

    void run();

    static void help(std::ostream& o) { o << "Compares two ODB files"; }

    static void usage(const std::string& name, std::ostream& o) {
        o << name
          << " [-excludeColumns <list-of-columns>] [-excludeColumnsTypes <list-of-columns>] [-dontCheckMissing] "
             "<file1.odb> <file2.odb>";
    }

private:

    // No copy allowed

    CompareTool(const CompareTool&);
    CompareTool& operator=(const CompareTool&);

    static char* dummyArgv_[];

    eckit::PathName file1_;
    eckit::PathName file2_;

    odc::RowsReaderIterator* reader1_;
    odc::RowsReaderIterator* reader2_;
};

}  // namespace tool
}  // namespace odc

#endif
