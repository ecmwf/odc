/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ToolFactory.cc
///
/// @author Piotr Kuchta, ECMWF, Feb 2009
///

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

#include "Exceptions.h"
#include "Tool.h"
#include "TestCase.h"
#include "ToolFactory.h"
#include "Regex.h"

namespace odb {
namespace tool {

map<string, AbstractToolFactory *> *AbstractToolFactory::toolFactories = 0;

class MatchAll : public vector<string> {
public:
	MatchAll() { push_back(".*"); }
};

const vector<string> AbstractToolFactory::matchAll = MatchAll();

test::TestCases* AbstractToolFactory::testCases(const vector<string> &patterns)
{
	ASSERT(toolFactories != 0);

	test::TestCases *v = new test::TestCases();

	for (map<string,AbstractToolFactory*>::iterator it = toolFactories->begin();
		it != toolFactories->end();
		it++)
	{
		string testName = it->first;
		if (testName.find("Test") == string::npos 
			|| !Tool::matchAny(patterns, testName))
			continue;

		AbstractToolFactory *factory = it->second;
		char *argv[] = {const_cast<char*>("test"), 0};
		Tool *tool = factory->create(1, argv);
		ASSERT(tool);

		test::TestCase *testCase = dynamic_cast<test::TestCase *>(tool);
		if (testCase != 0)
		{
			testCase->name(testName);
			v->push_back(testCase);
		}
		else
		{
			Log::warning() << "AbstractToolFactory::testCases: " << testName
				<< " is not a TestCase. Skipping" << endl;
			delete tool;
		}
	}
	return v;
}

AbstractToolFactory& AbstractToolFactory::findTool(const string &name)
{
	ASSERT(toolFactories);

	map<string,AbstractToolFactory*>::const_iterator it = toolFactories->find(name);

	ASSERT("Unknown tool" && it != toolFactories->end());

	return *it->second;
}

void AbstractToolFactory::printToolHelp(const string& name, ostream &s)
{
	findTool(name).help(s);
	s << endl;
}

void AbstractToolFactory::printToolUsage(const string& name, ostream &s)
{
	findTool(name).usage(name, s);
	s << endl;
}

void AbstractToolFactory::printToolsHelp(ostream &s)
{
	ASSERT(toolFactories);

	for (map<string,AbstractToolFactory*>::iterator it = toolFactories->begin();
		it != toolFactories->end();
		it++)
	{
		string toolName = it->first;
		AbstractToolFactory *toolFactory = it->second;
		
		if (toolName.find("Test") == string::npos && !toolFactory->experimental())
		{
				s << toolName << ":\t";
				toolFactory->help(s);
				s << endl << "Usage:" << endl << "\t";
				toolFactory->usage(toolName, s);
				s << endl << endl;
		}
	}
}

void AbstractToolFactory::listTools(ostream& s)
{
	ASSERT(toolFactories);

	for (map<string,AbstractToolFactory*>::iterator it = toolFactories->begin();
		it != toolFactories->end();
		it++)
	{
		string toolName = it->first;
		AbstractToolFactory *toolFactory = it->second;
		
		if (toolName.find("Test") == string::npos && !toolFactory->experimental())
		{
				s << "	" << toolName << "	";
				toolFactory->help(s);
				s << endl;
		}
	}
}

Tool* AbstractToolFactory::createTool(const string& name, int argc, char **argv)
{
	AbstractToolFactory *factory = (*toolFactories)[name];
	if (factory == 0)
		return 0;

	return factory->create(argc, argv);
};

AbstractToolFactory::AbstractToolFactory(const string& name)
{
	if (toolFactories == 0)
		toolFactories = new map<string, AbstractToolFactory *>();

	(*toolFactories)[name] = this;
}

AbstractToolFactory::~AbstractToolFactory()
{
	if (toolFactories)
	{
		delete toolFactories;
		toolFactories = 0;
	}
}

} // namespace tool 
} // namespace odb 

