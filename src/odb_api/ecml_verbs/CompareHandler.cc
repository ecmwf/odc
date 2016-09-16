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

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core//ExecutionContext.h"
#include "ecml/core/Environment.h"
#include "ecml/data/DataHandleFactory.h"

using namespace std;
using namespace eckit;
using namespace ecml;
using namespace odb;

namespace odb {

CompareHandler::CompareHandler(const string& name) : RequestHandler(name) {}

ecml::Values CompareHandler::handle(ExecutionContext& context)
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

    Log::info() << "No difference found" << endl;

    List list;
    return list;
}

} // namespace odb 

