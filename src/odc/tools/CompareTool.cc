/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/tools/CompareTool.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/StringTools.h"
#include "odc/Comparator.h"
#include "odc/Reader.h"

using namespace std;
using namespace eckit;

namespace odc {
namespace tool {

CompareTool::CompareTool(int argc, char* argv[]) : Tool(argc, argv) {
    registerOptionWithArgument("-excludeColumnsTypes");
    registerOptionWithArgument("-excludeColumns");
    if (parameters().size() != 3) {
        Log::error() << "Usage: ";
        usage(parameters(0), Log::error());
        Log::error() << std::endl;
        std::stringstream ss;
        ss << "Expected exactly 3 command line parameters";
        throw UserError(ss.str());
    }

    PathName p;
    if (!(p = PathName(parameters()[1])).exists() || !(p = PathName(parameters()[2])).exists()) {
        stringstream s;
        s << "File " << p << " does not exist.";
        throw Exception(s.str());
    }

    file1_ = parameters()[1];
    file2_ = parameters()[2];
}


void CompareTool::run() {
    Timer t(std::string("Comparing files ") + file1_ + " and " + file2_);
    odc::Reader oda1(file1_);
    odc::Reader oda2(file2_);

    odc::Reader::iterator it1(oda1.begin());
    odc::Reader::iterator end1(oda1.end());
    odc::Reader::iterator it2(oda2.begin());
    odc::Reader::iterator end2(oda2.end());

    std::vector<std::string> excludedColumnsTypes =
        StringTools::split(",", optionArgument("-excludeColumnsTypes", std::string("")));
    std::vector<std::string> excludedColumns =
        StringTools::split(",", optionArgument("-excludeColumns", std::string("")));

    if (excludedColumnsTypes.size()) {
        Log::info() << "excludedColumnsTypes:" << excludedColumnsTypes << std::endl;
    }

    if (excludedColumns.size()) {
        Log::info() << "excludedColumns:" << excludedColumns << std::endl;
    }

    bool checkMissing = !optionIsSet("-dontCheckMissing");
    odc::Comparator(checkMissing).compare(it1, end1, it2, end2, file1_, file2_, excludedColumnsTypes, excludedColumns);
}

}  // namespace tool
}  // namespace odc
