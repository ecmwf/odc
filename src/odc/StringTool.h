/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file StringTool.h
///
/// @author Piotr Kuchta, ECMWF, Oct 2010

#ifndef StringTool_H
#define StringTool_H

#include "eckit/eckit.h"
#include "odc/api/ColumnType.h"


namespace eckit {
class PathName;
class CodeLocation;
}  // namespace eckit

namespace odc {

class StringTool {

    typedef int (*ctypeFun)(int);

public:

    static std::string readFile(const eckit::PathName fileName, bool logging = false);
    static std::vector<std::string> readLines(const eckit::PathName fileName, bool logging = false);

    static void trimInPlace(std::string&);

    static bool match(const std::string& regex, const std::string&);
    static bool matchAny(const std::vector<std::string>& regs, const std::string&);

    static bool check(const std::string&, ctypeFun);
    static bool isInQuotes(const std::string&);
    static std::string unQuote(const std::string&);

    static double cast_as_double(const std::string&);
    static std::string double_as_string(double);
    // static std::string stringAsDouble(double v)

    static std::string int_as_double2string(double);

    static int shell(std::string cmd, const eckit::CodeLocation& where, bool assertSuccess = true);

    static double translate(const std::string& v);
    static std::string valueAsString(double, api::ColumnType);

    static std::string patchTimeForMars(const std::string& v);

    static bool isSelectStatement(const std::string&);
};

std::ostream& operator<<(std::ostream&, const std::vector<std::string>&);

}  // namespace odc

#endif
