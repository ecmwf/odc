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

#include <fstream>
#include <memory>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/StringTools.h"

#include "odc/CommandLineParser.h"
#include "odc/LibOdc.h"
#include "odc/tools/TestCase.h"
#include "odc/tools/TestRunner.h"
#include "odc/tools/Tool.h"
#include "odc/tools/ToolFactory.h"

using namespace eckit;
using namespace std;

namespace odc {
namespace tool {
namespace test {

TestRunner::TestRunner(CommandLineParser& clp) : clp_(clp), mars_sms_label_(false), label_() {
    if (getenv("MARS_SMS_LABEL")) {
        mars_sms_label_ = true;
        label_          = getenv("MARS_SMS_LABEL");
    }
}

TestRunner::~TestRunner() {}

void TestRunner::run() {
    ASSERT(getenv("odc_TEST_DATA_PATH") && "odc_TEST_DATA_PATH must be set");

    stringstream totalRunningTime;
    unique_ptr<Timer> allTestsTimer(new Timer("Total", totalRunningTime));
    unique_ptr<TestCases> tests;

    failed_.clear();

    if (clp_.parameters().size() == 1) {
        tests.reset(AbstractToolFactory::testCases());
        Log::info() << "clp_.parameters()" << clp_.parameters() << endl;
        runTests(*tests);
    }
    else {
        // TODO: keep the config in the odc_TEST_DATA_PATH
        // readConfig("../../../odc/src/odb/TestRunnerApplication.cfg");
        readConfig("/tmp/Dropbox/work/odc/src/odb/TestRunnerApplication.cfg");
        readConfig("/tmp/Dropbox/work/odc/src/tools/TestRunnerApplication.cfg");
        tests.reset(new TestCases());
        for (size_t i = 1; i < clp_.parameters().size(); ++i) {
            string suiteName = clp_.parameters()[i];
            ASSERT("Suite does not exist" && suites_.find(suiteName) != suites_.end());
            vector<string>& suite = suites_[suiteName];
            unique_ptr<vector<TestCase*> > tsts(AbstractToolFactory::testCases(suite));

            runTests(*tsts);
            tests->insert(tests->end(), tsts->begin(), tsts->end());
        }
    }

    allTestsTimer.reset();

    ofstream xmlf("testresults.xml");
    xmlf << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    xmlf << "<testsuite name=\"unittests\" time=\"" << StringTools::split(" ", totalRunningTime.str())[1] << "\">"
         << endl;
    xmlf << xml_.str();
    xmlf << "</testsuite>" << endl;

    size_t nTests = tests->size();
    for (size_t i = 0; i < nTests; ++i)
        delete (*tests)[i];

    if (failed_.size() == 0) {
        Log::info() << endl << "+- Phew, made it! All " << nTests << " tests passed successfully. " << endl << endl;
        Log::info() << runningTimes_.str() << endl;
        ;
        Log::info() << totalRunningTime.str() << endl;
    }
    else {
        Log::error() << endl << "+- Summary: " << failed_.size() << " test(s) failed." << endl;
        for (vector<FailedTest>::iterator it = failed_.begin(); it != failed_.end(); ++it) {
            const string& name = it->first;
            const string& what = it->second;
            Log::error() << "\t" << name << ": " << endl << what;
        }
        Log::error() << endl;

        stringstream ss;
        ss << " " << failed_.size() << " test(s) failed";
        throw eckit::SeriousBug(ss.str());
    }
}

void TestRunner::runTests(const TestCases& tests) {
    for (TestCases::const_iterator it = tests.begin(); it != tests.end(); ++it) {
        bool exceptionThrown = false;
        string what;
        TestCase* tst      = *it;
        const string& name = tst->name();

        Log::info() << "+- Running " << name << " ..." << endl;
        smslabel(name);

        stringstream runningTime;
        unique_ptr<Timer> timer(new Timer(name, runningTime));
        try {
            tst->setUp();
            tst->test();
        }
        catch (std::exception& e) {
            Log::warning() << "+- FAILED" << endl;
            exceptionThrown = true;
            what += string(e.what()) + '\n';
        }
        catch (...) {
            Log::warning() << "+- FAILED: unknown exception!" << endl;
            exceptionThrown = true;
            what += string("Uknown exception") + '\n';
        }
        try {
            tst->tearDown();
        }
        catch (std::exception& e) {
            Log::warning() << "+- Exception thrown from tearDown." << endl;
            exceptionThrown = true;
            what += string("[In tearDown:]") + string(e.what()) + '\n';
        }
        catch (...) {
            Log::warning() << "+- FAILED: unknown exception!" << endl;
            exceptionThrown = true;
            what += string("Uknown exception") + '\n';
        }

        if (exceptionThrown) {
            failed_.push_back(make_pair(name, what));
            xml_ << "<testcase classname=\"test\" name=\"" << name << "\">" << endl;
            xml_ << "	<failure type=\"exception\"><![CDATA[" << what << "]]></failure>" << endl;
            xml_ << "</testcase>" << endl;
        }
        else {
            timer.reset();
            runningTimes_ << runningTime.str();
            Log::info() << "+- Passed." << endl << endl;
            xml_ << "<testcase classname=\"test\" name=\"" << name << "\" time=\""
                 << StringTools::split(" ", runningTime.str())[1] << "\"/>" << endl;
        }
    }
}

void TestRunner::readConfig(const PathName fileName) {
    LOG_DEBUG_LIB(LibOdc) << "TestRunner::readConfig: reading file '" << fileName << "'" << endl;
    suites_.clear();

    vector<string> lines = StringTool::readLines(fileName);
    for (size_t i = 0; i < lines.size(); ++i) {
        vector<string> words = StringTools::split(":", lines[i]);
        if (words.size() == 0)
            continue;
        ASSERT("Each line of config file should be like: '<suite_name> : TestPattern1 TestPattern2 ...'" &&
               words.size() == 2);

        suites_[words[0]] = StringTools::split(" \t", words[1]);
        LOG_DEBUG_LIB(LibOdc) << "TestRunner::readConfig(\"" << fileName << "\"): " << words[0] << ": "
                              << suites_[words[0]].size() << " entries." << endl;
    }
}

void TestRunner::smslabel(const string& s) {
    if (!mars_sms_label_)
        return;
    string cmd = "smslabel ";
    cmd += label_ + " " + s;
    std::system(cmd.c_str());
}

}  // namespace test
}  // namespace tool
}  // namespace odc
