/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file Tool.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef Tool_H
#define Tool_H

#include "odc/CommandLineParser.h"
#include "odc/StringTool.h"

namespace eckit {
class PathName;
}
class Application;

namespace odc {
namespace tool {

class Tool : public StringTool, public CommandLineParser {
public:

    virtual void run() = 0;

    virtual ~Tool();

    std::string name() { return name_; }
    void name(const std::string& s) { name_ = s; }

    static void registerTools();

protected:

    Tool(int argc, char** argv);
    Tool(const CommandLineParser&);

private:

    std::string name_;
};


template <typename T>
struct ExperimentalTool {
    enum {
        experimental = false
    };
};

}  // namespace tool
}  // namespace odc

#endif
