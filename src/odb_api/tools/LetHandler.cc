/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/parser/Request.h"

#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"
#include "eckit/utils/Interpreter.h"
#include "eckit/utils/SpecialFormHandler.h"

#include "odb_api/tools/LetHandler.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {

LetHandler::LetHandler(const string& name)
: SpecialFormHandler(name)
{}

Request LetHandler::handle(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "let");

    Request evaluatedAttributes (context.interpreter().evalAttributes(request, context));
    Request frame (new Cell("_frame", "let", 0, 0));
    for (Request e(evaluatedAttributes->rest()); e; e = e->rest())
    {
        ASSERT(e->tag() == "");

        const string& name (e->text());
        Values values (context.interpreter().evalList(e->value(), context));

        frame->append(new Cell("", name, values, 0));
    }
    context.pushEnvironmentFrame(frame);
    return Cell::clone(frame);
}

} // namespace tool
} // namespace odb

