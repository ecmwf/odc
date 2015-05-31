/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ListHandler.h"

#include "eckit/parser/Request.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"

using namespace std;
using namespace eckit;

ListHandler::ListHandler(const string& name) : RequestHandler(name) {}

Values ListHandler::handle(ExecutionContext& context)
{
    Values r (Cell::clone(context.environment().lookup("values")));
    context.stack().push(r);
    return r;
}

