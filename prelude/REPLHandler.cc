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
#include "ecml/parser/Request.h"
#include "eckit/cmd/UserInput.h"

#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"
#include "ecml/core/Interpreter.h"

#include "ecml/ast/FunctionDefinition.h"
#include "ecml/ast/Closure.h"
#include "ecml/prelude/REPLHandler.h"
#include "ecml/prelude/Autocompleter.h"

using namespace std;

namespace ecml {

REPLHandler::REPLHandler(const string& name)
: SpecialFormHandler(name)
{}

Request REPLHandler::handle(const Request, ExecutionContext& context)
{
    repl(context);

    return new Cell("_list", "", 0, 0);
}


string REPLHandler::historyFile()
{
    return string (getenv("HOME")) + "/.ecml_history";
}

void REPLHandler::readHistory()
{
    eckit::UserInput::loadHistory(historyFile().c_str());
}


void REPLHandler::writeHistory()
{
    eckit::UserInput::saveHistory(historyFile().c_str());
}

string REPLHandler::readLine(ExecutionContext& context)
{
    // It would be nicer to pass it as a third param to getUserInput
    Autocompleter::context_ = &context; 
    const char* line (eckit::UserInput::getUserInput("ecml> ", eckit::UserInput::completion_proc (&Autocompleter::completion)));
    if (line == 0)
        return "quit";
    return line;
}

void REPLHandler::repl(ExecutionContext& context)
{
    readHistory();
    string cmd; 
    while (true)
    {
        cmd += readLine(context);

        if (cin.eof()
            || cmd == "quit"
            || cmd == "bye")
        {
            writeHistory();
            cout << "Bye." << endl;
            break;
        }

        if (! cmd.size()) continue;

        if (cmd.rfind("\\") == cmd.size() - 1)
        {
            cmd.erase(cmd.size() - 1);
            cmd += '\n';
            continue;
        }

        try {
            Values r (context.execute(cmd));
            cout << " => " << r << endl;
            if (showResultGraph(context))
                r->graph();
            delete r;
        } catch (Exception e) {
            // error message is already printed by Exception ctr
            //cout << "*** error: " << e.what() << endl;
        }
        cmd = "";
    }
}

bool REPLHandler::showResultGraph(ExecutionContext& context)
{
    if (! context.environment().lookupNoThrow("show_dot"))
        return false;
    else
    {
        vector<string> vs (context.environment().lookupList("show_dot", context));
        return vs.size() == 1 && vs[0] == "true";
    }
}

} // namespace ecml
