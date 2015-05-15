/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "CompareHandler.h"

#include "odb_api/Comparator.h"

#include "eckit/types/Types.h"
#include "eckit/parser/Request.h"
#include "eckit/parser/RequestParser.h"
#include "eckit/utils/ExecutionContext.h"

#include "eckit/utils/DataHandleFactory.h"

using namespace std;
using namespace eckit;
using namespace odb;

CompareHandler::CompareHandler(const string& name) : RequestHandler(name) {}

Values CompareHandler::handle(const Request& req, ExecutionContext& context)
//Values CompareHandler::handle(const Request& request)
{
    Request request(req);
    popIfNotSet("left", request, context);
    popIfNotSet("right", request, context);

    Values left (request.at("left")),
           right (request.at("right"));

    Log::info() << "left: " << left << endl;
    Log::info() << "right: " << right  << endl;

    if (left.size() != right.size())
        throw UserError("Sizes of lists differ");

    Comparator comparator;
    for (size_t i(0); i < left.size(); ++i)
    {
        string leftString (left[i]), rightString (right[i]);
        Log::info() << "Comparing" << endl << " " << leftString << endl << "to" << endl << " " << rightString << endl;

        {
            auto_ptr<DataHandle> l (DataHandleFactory::openForRead(leftString)),
                                 r (DataHandleFactory::openForRead(rightString));
            comparator.compare(*l, *r);
        }
    }
    Log::info() << "No difference found" << endl;

    // For now this is not returning anything, just throwing if a difference found
    return Values();
}

Values CompareHandler::handle(const Request& request)
//void CompareHandler::handle(const Request& request, ExecutionContext& context)
{
    NOTIMP; // FIXME
    Request req(request);
    //popIfNotSet("left", req, context);
    //popIfNotSet("right", req, context);
    handle(req);
}

