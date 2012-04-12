/// \file CommandLineParser.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#include <iostream>
using namespace std;

#include "Application.h"
#include "Translator.h"

#include "CommandLineParser.h"

namespace odb {
namespace tool {

CommandLineParser::~CommandLineParser() {}

CommandLineParser::CommandLineParser(int argc, char **argv)
: app_(0), commandLineParsed_(false), argc_(argc), argv_(argv), registeredOptionsWithArguments_(), optionsWithArguments_(), optionsNoArguments_(), parameters_()
{}

CommandLineParser::CommandLineParser(const Application *app)
: app_(app), commandLineParsed_(false), argc_(0), argv_(0), registeredOptionsWithArguments_(), optionsWithArguments_(), optionsNoArguments_(), parameters_()
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

int CommandLineParser::argc() { return app_ ? app_->argc() : argc_; }

string CommandLineParser::argv(int i) { return app_ ? app_->argv(i) : argv_[i]; }

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
		if (s[0] != '-')
		{
			parameters_.push_back(s);
			Log::debug() << "CommandLineParser::parseCommandLine: parameter: " << s << endl;
		}
		else
		{
			if (registeredOptionsWithArguments_.find(s) != registeredOptionsWithArguments_.end())
			{	
				optionsWithArguments_[s] = argv(++i);
				Log::debug() << "CommandLineParser::parseCommandLine: option with argument: "
					<< s << " = " << optionsWithArguments_[s] << endl;
			}
			else
			{
				optionsNoArguments_.insert(s);
				Log::debug() << "CommandLineParser::parseCommandLine: option with no argument: " << s << endl;
			}
		}
	}
	commandLineParsed_ = true;
}

void CommandLineParser::print(ostream& s) const
{
	for (set<std::string>::const_iterator i = optionsNoArguments_.begin(); i != optionsNoArguments_.end(); ++i)
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

