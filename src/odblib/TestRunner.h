/// \file TestRunner.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestRunner_H
#define TestRunner_H

#include "CommandLineParser.h"

namespace odb {
namespace tool {
namespace test {

class TestRunner {
public:
	TestRunner (CommandLineParser&);
	virtual ~TestRunner ();

	size_t numberOfFailed() { return failed_.size(); }

	void run();

private:
	typedef pair<string, string> FailedTest;
	typedef map<string, vector<string> > Suites;

	void readConfig(const PathName fileName);
	void runTests(const TestCases &tests);

	void smslabel(const string &);

	CommandLineParser clp_;

	Suites suites_;
	vector<FailedTest> failed_;
	stringstream runningTimes_;

	bool mars_sms_label_;
	string label_;
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif
