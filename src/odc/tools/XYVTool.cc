/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/tools/XYVTool.h"

#include <fstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "odc/Select.h"

using namespace std;
using namespace eckit;

namespace odc {
namespace tool {

XYVTool::XYVTool(int argc, char** argv) : Tool(argc, argv) {}

XYVTool::~XYVTool() {}

void XYVTool::run() {
    if (parameters().size() != 4) {
        Log::error() << "Usage: ";
        usage(parameters(0), Log::error());
        Log::error() << std::endl;
        std::stringstream ss;
        ss << "Expected exactly 4 command line parameters";
        throw UserError(ss.str());
    }

    PathName inputFile      = parameters(1);
    std::string valueColumn = parameters(2);
    PathName outputFile     = parameters(3);

    std::ofstream out;
    out.open(outputFile.asString().c_str());

    out << "#GEO" << std::endl << std::endl;
    out << "#FORMAT XYV" << std::endl << std::endl;
    out << "PARAMETER = 12004" << std::endl << std::endl;
    out << "x/long	y/lat	value" << std::endl;
    out << "#DATA" << std::endl << std::endl;

    std::string select = std::string("select lat, lon, ") + valueColumn + " from \"" + inputFile + "\";";
    Log::info() << select << std::endl;

    odc::Select oda(select);
    for (odc::Select::iterator it = oda.begin(); it != oda.end(); ++it) {
        out << (*it)[0] << "\t" << (*it)[1] << "\t" << (*it)[2] << std::endl;
    }
    out.close();
}

}  // namespace tool
}  // namespace odc
