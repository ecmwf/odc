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

#include "ecml/parser/Request.h"
#include "ecml/ExecutionContext.h"
#include "ecml/Environment.h"

using namespace std;
using namespace eckit;

ListHandler::ListHandler(const string& name) : RequestHandler(name) {}

Values ListHandler::handle(ExecutionContext& context)
{
    Values r (Cell::clone(context.environment().lookup("values")));
    return r;
}

