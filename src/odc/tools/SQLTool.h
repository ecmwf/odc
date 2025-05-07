/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Aug 2018

#ifndef odc_SQLTool_H
#define odc_SQLTool_H

#include <memory>

#include "eckit/sql/SQLOutputConfig.h"
#include "odc/tools/Tool.h"

namespace eckit {
class Offset;
class Length;
namespace sql {
class SQLSession;
class SQLParser;
class SQLOutputConfig;
}  // namespace sql
}  // namespace eckit

namespace odc {

namespace sql {
class SQLOutputConfig;
}

namespace tool {

//----------------------------------------------------------------------------------------------------------------------

class SQLTool : public Tool {

public:

    SQLTool(int argc, char** argv);
    virtual ~SQLTool();

    virtual void run();

    static void help(std::ostream& o) { o << "Executes SQL statement"; }
    static void usage(const std::string& name, std::ostream& o) {
        o << name << " <select-statement> | <script-filename>" << std::endl;
        o << "             [-T]                        Disables printing of column names" << std::endl;
        o << "             [-offset <offset>]          Start processing file at a given offset" << std::endl;
        o << "             [-length <length>]          Process only given bytes of data" << std::endl;
        o << "             [-N]                        Do not write NULLs, but proper missing data values" << std::endl;
        o << "             [-i <inputfile>]            ODB input file" << std::endl;
        o << "             [-o <outputfile>]           ODB output file" << std::endl;
        o << "             [-f default|wide|ascii|odb] ODB output format (odb is binary ODB, ascii and wide are ascii "
             "formatted with bitfield definitions in header. Default is ascii on stdout and odb to file)"
          << std::endl;
        o << "             [-delimiter <delim>]        Changes the default values' delimiter (TAB by default)"
          << std::endl;
        o << "                                         delim can be any character or string" << std::endl;
        o << "             [--binary|--bin]            Print bitfields in binary notation" << std::endl;
        o << "             [--no_alignment]            Do not align columns" << std::endl;
        o << "             [--full_precision]          Print with full precision" << std::endl;
    }

private:

    std::unique_ptr<odc::sql::SQLOutputConfig> sqlOutputConfig_;

    std::string inputFile_;  // -i
    eckit::Offset offset_;   // -offset
    eckit::Length length_;   // -length
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace tool
}  // namespace odc

#endif
