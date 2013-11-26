/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// \file CommandLineParser.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#include <iostream>

#include "eckit/runtime/Context.h"
#include "eckit/parser/Translator.h"

#include "odblib/CommandLineParser.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {

CommandLineParser::~CommandLineParser() {}

CommandLineParser::CommandLineParser(int argc, char **argv)
: commandLineParsed_(false),
  argc_(argc),
  argv_(argv),
  registeredOptionsWithArguments_(),
  optionsWithArguments_(),
  optionsNoArguments_(),
  parameters_()
{}

CommandLineParser::CommandLineParser(const CommandLineParser& other)
{
	CommandLineParser& self = *this;

	self.registeredOptionsWithArguments_ = other.registeredOptionsWithArguments_;
	self.parameters_ = const_cast<CommandLineParser&>(other).parameters();
	self.optionsWithArguments_ = other.optionsWithArguments_;
	self.optionsNoArguments_ = other.optionsNoArguments_;
	self.commandLineParsed_ = true;

	ASSERT(self.commandLineParsed_ == other.commandLineParsed_);
}

CommandLineParser& CommandLineParser::operator=(const CommandLineParser& other)
{
	if (this == &other) return *this;

	CommandLineParser& self = *this;

	self.registeredOptionsWithArguments_ = other.registeredOptionsWithArguments_;
	self.parameters_ = const_cast<CommandLineParser&>(other).parameters();
	self.optionsWithArguments_ = other.optionsWithArguments_;
	self.optionsNoArguments_ = other.optionsNoArguments_;
	self.commandLineParsed_ = true;

	ASSERT(self.commandLineParsed_ == other.commandLineParsed_);

	return *this;
}

int CommandLineParser::argc() { return argc_; }

string CommandLineParser::argv(int i)
{
	if (i >= argc_)
	{
		stringstream ss;
		ss << "Expected at least " << i << " command line parameters";
		throw eckit::UserError(ss.str());
	}
	return argv_[i];
}

void CommandLineParser::registerOptionWithArgument(const std::string& option)
{
	registeredOptionsWithArguments_.insert(string(option));
}

const vector<std::string> CommandLineParser::parameters()
{
	if (! commandLineParsed_) parseCommandLine();
	return parameters_;
}

bool CommandLineParser::optionIsSet(const std::string& option)
{
	if (! commandLineParsed_) parseCommandLine();

	if (optionsNoArguments_.find(option) != optionsNoArguments_.end())
		return true;

	return optionsWithArguments_.find(option) != optionsWithArguments_.end();
}

template <typename T>
T CommandLineParser::optionArgument(const std::string& option, T defaultValue)
{
	if (! commandLineParsed_) parseCommandLine();

	map<std::string, std::string>::iterator it = optionsWithArguments_.find(option);
	if (it == optionsWithArguments_.end())
		return defaultValue;

	Translator<std::string, T> translator;
	return translator(it->second);
}

void CommandLineParser::parseCommandLine()
{
	for (int i = 0; i < argc(); ++i)
	{
		string s = argv(i);
		if (s[0] != '-' || s.size() == 1)
		{
			parameters_.push_back(s);
			Log::debug() << "CommandLineParser::parseCommandLine: parameter: " << s << std::endl;
		}
		else
		{
			if (registeredOptionsWithArguments_.find(s) != registeredOptionsWithArguments_.end())
			{	
				optionsWithArguments_[s] = argv(++i);
				Log::debug() << "CommandLineParser::parseCommandLine: option with argument: "
					<< s << " = " << optionsWithArguments_[s] << std::endl;
			}
			else
			{
				optionsNoArguments_.insert(s);
				Log::debug() << "CommandLineParser::parseCommandLine: option with no argument: " << s << std::endl;
			}
		}
	}
	commandLineParsed_ = true;
}

void CommandLineParser::print(std::ostream& s) const
{
	for (std::set<std::string>::const_iterator i = optionsNoArguments_.begin(); i != optionsNoArguments_.end(); ++i)
		s << *i << "  ";

	for (map<std::string, std::string>::const_iterator i = optionsWithArguments_.begin(); i != optionsWithArguments_.end(); ++i)
		s << i->first << " " << i->second << "  ";

	for (size_t i = 0; i < parameters_.size(); ++i)
		s << parameters_[i] << "  ";

}

// Template function's explicit instantiations.

template std::string CommandLineParser::optionArgument(const std::string&, std::string);
template int CommandLineParser::optionArgument(const std::string&, int);
template long CommandLineParser::optionArgument(const std::string&, long);
template double CommandLineParser::optionArgument(const std::string&, double);

} // namespace tool 
} // namespace odb 

