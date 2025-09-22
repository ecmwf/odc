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

#ifndef CommandLineParser_H
#define CommandLineParser_H

#include <map>
#include <set>
#include <string>
#include <vector>


namespace odc {
namespace tool {

class CommandLineParser {
public:

    CommandLineParser(int argc, char** argv);

    CommandLineParser(const CommandLineParser&);
    CommandLineParser& operator=(const CommandLineParser&);

    virtual ~CommandLineParser();

    void registerOptionWithArgument(const std::string&);

    /// @return command line parameters (without the options starting with '-')
    const std::vector<std::string> parameters();
    std::string parameters(size_t i) { return parameters()[i]; }

    /// @return true if argumentless option is set
    bool optionIsSet(const std::string&);

    /// @return value of the command line option passed to the tool converted
    ///         to given type, or value of the second parameter if option not
    ///         present on command line.
    template <typename T>
    T optionArgument(const std::string&, T defaultValue);

    int argc();
    std::string argv(int i);
    char** argv() { return argv_; }

private:

    void parseCommandLine();


    bool commandLineParsed_;
    int argc_;
    char** argv_;

    std::set<std::string> registeredOptionsWithArguments_;

    std::map<std::string, std::string> optionsWithArguments_;
    std::set<std::string> optionsNoArguments_;
    std::vector<std::string> parameters_;

    void print(std::ostream& s) const;
    friend std::ostream& operator<<(std::ostream& s, const CommandLineParser& o) {
        o.print(s);
        return s;
    }
};

}  // namespace tool
}  // namespace odc

#endif
