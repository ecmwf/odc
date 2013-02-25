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

	void readConfig(const eclib::PathName fileName);
	void runTests(const TestCases &tests);

	void smslabel(const string &);

	CommandLineParser clp_;

	Suites suites_;
	vector<FailedTest> failed_;
	stringstream runningTimes_;
	stringstream xml_;

	bool mars_sms_label_;
	string label_;
};

} // namespace test
} // namespace tool 
} // namespace odb 

#endif
