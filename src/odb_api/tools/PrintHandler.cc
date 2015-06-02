/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "PrintHandler.h"

#include "eckit/parser/Request.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"
#include "eckit/utils/Interpreter.h"

using namespace std;
using namespace eckit;

PrintHandler::PrintHandler(const string& name, const string& end)
: RequestHandler(name),
  end_(end)
{}

Values PrintHandler::handle(ExecutionContext& context)
{
    Values r (Cell::clone(context.environment().lookupNoThrow("values")));
    if (!r )
        r = new Cell("_list", "", 0, 0);
    else
        for (Request e(r); e; e = e->rest())
            if (e->value())
                cout << Interpreter::eval(e->value(), context) << " ";
            else
                cout << "NULL" << " ";

    cout << end_;
    return r;
}

