/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/ecml/parser/Request.h"
#include "eckit/cmd/UserInput.h"

#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/Interpreter.h"

#include "eckit/ecml/ast/FunctionDefinition.h"
#include "eckit/ecml/ast/Closure.h"
#include "eckit/ecml/prelude/Autocompleter.h"

using namespace std;

namespace eckit {

ExecutionContext* Autocompleter::context_ (0);

bool Autocompleter::notInWord(char p)
{
    return p == ' '
        || p == '\t'
        || p == ','
        || p == '('
        || p == ')'
        || p == '/'
        || p == '='
        ;
}

std::string Autocompleter::suffix(const char* line, int pos)
{
    char *p (const_cast<char*>(line) + pos);
    while (p != line && !notInWord(*(p - 1)))
        --p;
    return p;
}

void Autocompleter::describe(const string& v, Cell* o)
{
    stringstream ss;
    ss << v << ": " << (o ? o->str() : "NULL");

    const string description (ss.str());

    ::write(1, "\r\n", 2);
    ::write(1, description.c_str(), description.size());
    ::write(1, "\r\n", 2);
}

bool Autocompleter::completion(const char* line, int pos, char* insert, int insertmax)
{
    ExecutionContext& context (*Autocompleter::context_); // it would be nice to have it passed as a parameter here

    const string prefix (suffix(line, pos));

    const vector<string> matchedVars (context.environment().lookupVariables("^" + prefix));
    const set<string> matched (matchedVars.begin(), matchedVars.end());

    if (matched.empty())
        return true;

    if (matched.size() == 1)
    {
        const string& match (*matched.begin());
        describe(match, context.environment().lookupNoThrow(match)); 

        for (size_t i(prefix.size()), ii(0); ii < insertmax && i < match.size(); ++i)
            insert[ii++] = match[i];

        return true;
    }

    size_t i(0);
    for (set<string>::const_iterator it (matched.begin()); it != matched.end(); ++it)
    {
        const string& ins (*it);
        for (size_t j(0); i < insertmax && j < ins.size(); ++j)
            insert[i++] = ins[j];

        if (i < insertmax)
            insert[i++] = ' ';
    }

    return false;
}

} // namespace eckit
