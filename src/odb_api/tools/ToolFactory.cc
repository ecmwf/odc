/*
 * (C) Copyright 1996-2012 ECMWF.
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

//#include <iostream>
//#include <string>
//#include <map>
//#include <vector>

//#include "eckit/utils/Regex.h"
#include "eckit/exception/Exceptions.h"

#include "Tool.h"
#include "TestCase.h"
#include "ToolFactory.h"

using namespace std;
using namespace eckit;

namespace odc {
namespace tool {

std::map<std::string, AbstractToolFactory *> *AbstractToolFactory::toolFactories = 0;

class MatchAll : public std::vector<std::string> {
public:
	MatchAll() { push_back(".*"); }
};

const std::vector<std::string> AbstractToolFactory::matchAll = MatchAll();

std::vector<test::TestCase*>* AbstractToolFactory::testCases(const std::vector<std::string> &patterns)
{
	ASSERT(toolFactories != 0);

	std::vector<test::TestCase*> *v = new std::vector<test::TestCase*>();

	for (std::map<std::string,AbstractToolFactory*>::iterator it = toolFactories->begin();
		it != toolFactories->end();
		it++)
	{
		std::string testName = it->first;
		if (testName.find("Test") == std::string::npos 
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
				<< " is not a TestCase. Skipping" << std::endl;
			delete tool;
		}
	}
	return v;
}

AbstractToolFactory& AbstractToolFactory::findTool(const std::string &name)
{
	ASSERT(toolFactories);

	std::map<std::string,AbstractToolFactory*>::const_iterator it = toolFactories->find(name);

	ASSERT("Unknown tool" && it != toolFactories->end());

	return *it->second;
}

void AbstractToolFactory::printToolHelp(const std::string& name, std::ostream &s)
{
	findTool(name).help(s);
	s << std::endl;
}

void AbstractToolFactory::printToolUsage(const std::string& name, std::ostream &s)
{
	findTool(name).usage(name, s);
	s << std::endl;
}

void AbstractToolFactory::printToolsHelp(std::ostream &s)
{
	ASSERT(toolFactories);

	for (std::map<std::string,AbstractToolFactory*>::iterator it = toolFactories->begin();
		it != toolFactories->end();
		it++)
	{
		std::string toolName = it->first;
		AbstractToolFactory *toolFactory = it->second;
		
		if (toolName.find("Test") == std::string::npos && !toolFactory->experimental())
		{
				s << toolName << ":\t";
				toolFactory->help(s);
				s << std::endl << "Usage:" << std::endl << "\t";
				toolFactory->usage(toolName, s);
				s << std::endl << std::endl;
		}
	}
}

void AbstractToolFactory::listTools(std::ostream& s)
{
	ASSERT(toolFactories);

	for (std::map<std::string,AbstractToolFactory*>::iterator it = toolFactories->begin();
		it != toolFactories->end();
		it++)
	{
		std::string toolName = it->first;
		AbstractToolFactory *toolFactory = it->second;
		
		if (toolName.find("Test") == std::string::npos && !toolFactory->experimental())
		{
				s << "	" << toolName << "	";
				toolFactory->help(s);
				s << std::endl;
		}
	}
}

Tool* AbstractToolFactory::createTool(const std::string& name, int argc, char **argv)
{
	AbstractToolFactory *factory = (*toolFactories)[name];
	if (factory == 0)
		return 0;

	return factory->create(argc, argv);
};

AbstractToolFactory::AbstractToolFactory(const std::string& name)
{
	if (toolFactories == 0)
		toolFactories = new std::map<std::string, AbstractToolFactory *>();

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
} // namespace odc 

