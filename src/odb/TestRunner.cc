/*
 * © Copyright 1996-2012 ECMWF.
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

#include <sstream>

#include "eclib/Application.h"
#include "eclib/PathName.h"
#include "eclib/FileHandle.h"
#include "eclib/Timer.h"
#include "eclib/Tokenizer.h"

#include "Tool.h"
#include "TestCase.h"
#include "ToolFactory.h"
#include "TestRunner.h"

namespace odb {
namespace tool {
namespace test {

TestRunner::TestRunner (int argc, char **argv)
: clp_(argc, argv),
  mars_sms_label_(false),
  label_()
{
	if (getenv("MARS_SMS_LABEL"))
	{
		mars_sms_label_ = true;
		label_ = getenv("MARS_SMS_LABEL");
	}
}

TestRunner::~TestRunner () {}

void TestRunner::run()
{
	auto_ptr<Timer> allTestsTimer(new Timer("Total"));
	auto_ptr<TestCases> tests(0);
	
	failed_.clear();

	if (clp_.parameters().size() == 1)
	{
		tests.reset(AbstractToolFactory::testCases());
		runTests(*tests);
	}
	else
	{
		readConfig("TestRunnerApplication.cfg");
		tests.reset(new TestCases());
		for (size_t i = 1; i < clp_.parameters().size(); ++i)
		{
			string suiteName = clp_.parameters()[i];
			ASSERT("Suite does not exist" && suites_.find(suiteName) != suites_.end());
			vector<string>& suite = suites_[suiteName];
			auto_ptr<TestCases> tsts(AbstractToolFactory::testCases(suite));
			runTests(*tsts);
			tests->insert(tests->end(), tsts->begin(), tsts->end());
		}
	}

	size_t nTests = tests->size();
	for (size_t i = 0; i < nTests; ++i)
		delete (*tests)[i];

	if (failed_.size() == 0) {
		Log::info() << endl << "+- Phew, made it! All " << nTests << " tests passed successfully." << endl;
		allTestsTimer.reset();
		Log::info() << runningTimes_.str();
	}
	else
	{
		Log::error() << endl << "+- Summary: " << failed_.size() << " test(s) failed." << endl;
		for (vector<FailedTest>::iterator it = failed_.begin(); it != failed_.end(); ++it) {
			string name = it->first;
			string what = it->second;
			Log::error() << "\t" << name << ": " << endl << what;
		}
		Log::error() << endl;

		stringstream ss;
		ss << " " << failed_.size() << " test(s) failed";
		throw SeriousBug(ss.str());
	}
}

void TestRunner::runTests(const TestCases& tests)
{
	for (TestCases::const_iterator it = tests.begin(); it != tests.end(); ++it)
	{
		bool exceptionThrown = false;
		string what;
		TestCase *tst = *it;
		string name = tst->name();

		Log::info() << "+- Running " << name << " ..." << endl;
		smslabel(name);

		stringstream runningTime;
		auto_ptr<Timer> timer(new Timer(name, runningTime));
		try {
			tst->setUp();
			tst->test();
		} catch (std::exception &e) {
			Log::warning() << "+- FAILED" << endl;
			exceptionThrown = true;
			what += string(e.what()) + '\n';
		} catch (...) {
			Log::warning() << "+- FAILED: unknown exception!" << endl;
			exceptionThrown = true;
			what += string("Uknown exception") + '\n';
		}
		try {
			tst->tearDown();
		} catch (std::exception &e) {
			Log::warning() << "+- Exception thrown from tearDown." << endl;
			exceptionThrown = true;
			what += string("[In tearDown:]") + string(e.what()) + '\n';
		} catch (...) {
			Log::warning() << "+- FAILED: unknown exception!" << endl;
			exceptionThrown = true;
			what += string("Uknown exception") + '\n';
		}

		if (exceptionThrown) {
			failed_.push_back(make_pair(name, what));
		}
		else {
			timer.reset();
			runningTimes_ << runningTime.str();
			
			Log::info() << "+- Passed." << endl << endl;
		}
	}
}

void TestRunner::readConfig(const PathName fileName)
{
	Log::debug() << "TestRunner::readConfig: reading file '" << fileName << "'" << endl;
	suites_.clear();

    vector<string> lines = StringTool::readLines(fileName);
    for (size_t i = 0; i < lines.size(); ++i)
	{
		vector<string> words = Tool::split(":", lines[i]);
		if (words.size() == 0)
			continue;
		ASSERT("Each line of config file should be like: '<suite_name> : TestPattern1 TestPattern2 ...'" && words.size() == 2);

		suites_[words[0]] = Tool::split(" \t", words[1]);
		Log::debug() << "TestRunner::readConfig(\"" << fileName << "\"): "
			<< words[0] << ": "
			<< suites_[words[0]].size() << " entries." << endl;
	}
}

void TestRunner::smslabel(const string &s)
{
	if (! mars_sms_label_)
		return;
	string cmd = "smslabel ";
	cmd += label_ + " " + s;
	system(cmd.c_str());
}

} // namespace test
} // namespace tool 
} // namespace odb 

