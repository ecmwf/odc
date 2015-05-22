/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "DuplicateHandler.h"

#include "eckit/parser/Request.h"
#include "eckit/utils/ExecutionContext.h"

using namespace std;
using namespace eckit;

DuplicateHandler::DuplicateHandler(const string& name) : RequestHandler(name) {}

Values DuplicateHandler::handle(const Request request)
{
    throw UserError("Duplicate makes sense in a context only"); // ?
    return Values();
}

Values DuplicateHandler::handle(const Request request, ExecutionContext& context)
{
    Values r (context.stack().top());

    ASSERT(r && "NULL on stack");

    Request clone (Cell::clone(r));
    context.stack().push(clone);
    return clone;
}

