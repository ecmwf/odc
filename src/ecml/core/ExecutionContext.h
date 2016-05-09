/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef eckit_utils_ExecutionContext_H
#define eckit_utils_ExecutionContext_H

#include "ecml/parser/Request.h"

namespace ecml {

class Environment;
class Module;
class Interpreter;
class RequestHandler;

class ExecutionContext {
public:
    ExecutionContext();
    ExecutionContext(const ExecutionContext&);
    ~ExecutionContext();

    std::vector<std::string> getValueAsList(const std::string& keyword);

    void registerHandler(const char*, ecml::RequestHandler&);
    void registerHandler(const std::string&, ecml::RequestHandler&);

    ecml::Values execute(const std::string&);
    ecml::Values executeScriptFile(const std::string&);

    void import(Module&);

    /// Creates a new environment frame and pushes it onto stack.
    void pushEnvironmentFrame();

    void pushEnvironmentFrame(ecml::Request);
    void popEnvironmentFrame();
    void popEnvironmentFrame(ecml::Request);

    Environment& environment();

    void interpreter(Interpreter*);
    Interpreter& interpreter() const;

    Cell* copy() const;

private:
    Environment* environment_;
    Environment* otherEnvironment_;
    Interpreter* interpreter_;
};

} // namespace ecml

#endif
