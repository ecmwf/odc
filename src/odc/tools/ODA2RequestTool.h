/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODA2RequestTool_H
#define ODA2RequestTool_H

#include "odc/tools/Tool.h"

namespace odc {
namespace tool {

class ODA2RequestTool : public Tool {
    typedef std::string Value;
    typedef std::set<Value> Values;

public:

    ODA2RequestTool();
    ODA2RequestTool(int argc, char** argv);
    ~ODA2RequestTool();

    static void help(std::ostream& o);
    static void usage(const std::string& name, std::ostream& o);

    virtual void run();

    void readConfig();
    void readConfig(const eckit::PathName&);
    void parseConfig(const std::string&);

    std::string generateMarsRequest(const eckit::PathName& inputFile, bool fast = false);

protected:

    std::vector<Values>& values() { return values_; }

    void gatherStats(const eckit::PathName& inputFile);
    std::string gatherStatsFast(const eckit::PathName& inputFile);

    eckit::PathName config();

private:

    std::map<std::string, std::string> columnName2requestKey_;
    std::vector<Values> values_;
};

}  // namespace tool
}  // namespace odc

#endif
