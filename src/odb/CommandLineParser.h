/// \file CommandLineParser.h
///
/// @author Piotr Kuchta, ECMWF, July 2009

#ifndef CommandLineParser_H
#define CommandLineParser_H

#include <map>
#include <set>
#include <vector>

using namespace std;

class Application;

namespace odb {
namespace tool {

class CommandLineParser {
public:
	CommandLineParser(int argc, char **argv);
	CommandLineParser(const Application *);

    CommandLineParser(const CommandLineParser&);
    CommandLineParser& operator=(const CommandLineParser&);

	virtual ~CommandLineParser();

	void registerOptionWithArgument(const std::string&);

	/// @return command line parameters (without the options starting with '-')
	const vector<std::string> parameters();
	std::string parameters(size_t i) { return parameters()[i]; }

	/// @return true if argumentless option is set
	bool optionIsSet(const std::string&);

	/// @return value of the command line option passed to the tool converted 
	///         to given type, or value of the second parameter if option not
	///         present on command line.
	template <typename T> T optionArgument(const std::string&, T defaultValue);

private:
	void parseCommandLine();

	int argc();
	string argv(int i);

	const Application *app_;

	bool commandLineParsed_;
	int argc_;
	char **argv_;

	set<std::string> registeredOptionsWithArguments_;

	map<std::string, std::string> optionsWithArguments_;
	set<std::string> optionsNoArguments_;
	vector<std::string> parameters_;

	void print(ostream& s) const;
	friend ostream& operator<<(ostream& s, const CommandLineParser& o) { o.print(s);  return s; }
};

} // namespace tool 
} // namespace odb 

#endif

