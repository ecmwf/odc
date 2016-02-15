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
#include "eckit/io/MultiHandle.h"

#include "eckit/types/Types.h"
#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/parser/RequestParser.h"
#include "experimental/eckit/ecml/core//ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/data/DataHandleFactory.h"

using namespace std;
using namespace eckit;
using namespace odb;

namespace odb {

CompareHandler::CompareHandler(const string& name) : RequestHandler(name) {}

Values CompareHandler::handle(ExecutionContext& context)
{
    vector<string> left (context.environment().lookupList("left", context));
    vector<string> right (context.environment().lookupList("right", context));

    Log::info() << "left: " << left << endl;
    Log::info() << "right: " << right  << endl;


    MultiHandle leftH, rightH;
    DataHandleFactory::buildMultiHandle(leftH, left);
    DataHandleFactory::buildMultiHandle(rightH, right);

    leftH.openForRead();
    rightH.openForRead();

    Comparator comparator;
    comparator.compare(leftH, rightH);
    /*

    if (left.size() != right.size())
        throw UserError("Sizes of lists differ");

    Comparator comparator;
    for (size_t i(0); i < left.size(); ++i)
    {
        string leftString (left[i]), rightString (right[i]);
        Log::info() << "Comparing" << endl << " " << leftString << endl << "to" << endl << " " << rightString << endl;

        {
            //auto_ptr<DataHandle> l (DataHandleFactory::openForRead(leftString)),
            //                     r (DataHandleFactory::openForRead(rightString));
            // TODO: fix leak. we probably cannot just use 'delete' here - I get:
            //    free(): invalid pointer: 0x0000000000670548 ***
            // on bamboo with the auto_ptr, perhaps because the dataHandle was allocated in a different library???
            DataHandle* l (DataHandleFactory::openForRead(leftString));
            DataHandle* r (DataHandleFactory::openForRead(rightString));
            comparator.compare(*l, *r);
        }
    }
    */
    Log::info() << "No difference found" << endl;

    Request r(new Cell("_list", "", 0, 0));

    return r;
}

} // namespace odb 

