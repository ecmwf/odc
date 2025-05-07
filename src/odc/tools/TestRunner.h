/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestRunner.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestRunner_H
#define TestRunner_H

#include "eckit/filesystem/PathName.h"
#include "odc/CommandLineParser.h"

namespace odc {
namespace tool {
namespace test {

class TestRunner {
public:

    TestRunner(CommandLineParser&);
    virtual ~TestRunner();

    size_t numberOfFailed() { return failed_.size(); }

    void run();

private:

    typedef std::pair<std::string, std::string> FailedTest;
    typedef std::map<std::string, std::vector<std::string> > Suites;

    void readConfig(const eckit::PathName fileName);
    void runTests(const TestCases& tests);

    void smslabel(const std::string&);

    CommandLineParser clp_;

    Suites suites_;
    std::vector<FailedTest> failed_;
    std::stringstream runningTimes_;
    std::stringstream xml_;

    bool mars_sms_label_;
    std::string label_;
};

}  // namespace test
}  // namespace tool
}  // namespace odc

#endif
