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

#include "eckit/io/FileHandle.h"
#include "eckit/parser/StringTools.h"

#include "metkit/MarsRequestHandle.h"
#include "metkit/DHSProtocol.h"

#include "odb_api/data/MarsHandleFactory.h"

using namespace eckit;
using namespace std;

/// @note This file is the only reason that metkit is required in odb_api
///       If MarsRequestHandle functionality is not required, that dependency can be dropped.

namespace odb {

MarsHandleFactory::MarsHandleFactory()
: DataHandleFactory("mars")
{}

bool shortName(const std::string& prefix, const std::string& s)
{
    if (prefix.size() > s.size())
        return false;
    return std::equal(prefix.begin(), prefix.end(), s.begin());
}

//std::string verb(const ecml::Request request)
//{
//    std::string v (eckit::StringTools::lower(request->text()));
//
//    const char* verbs[] = {"retrieve", "stage", "list", "archive", 0};
//
//    for (size_t i (0); verbs[i]; ++i)
//        if (shortName(v, verbs[i]))
//            return verbs[i];
//
//    throw eckit::UserError(std::string("Unknown request '") + v + "'");
//    return v;
//}

DataHandle* MarsHandleFactory::makeHandle(const string& r) const
{
    //Log::debug() << "MarsHandleFactory::makeHandle: parsing " << r << endl;

    //ecml::Request requests (ecml::RequestParser::parse(r));

    //Log::debug() << "MarsHandleFactory::makeHandle: requests = " << requests << endl;

    //ecml::Request request (requests->value());

    //Log::debug() << "MarsHandleFactory::makeHandle: request = " << request << endl;

    //if (requests->rest())
    //    Log::warning() << "MarsHandleFactory: Only " << request << " used, skipped rest of " << requests << endl;

    //string host (ecml::RequestHandler::database(request));
    //long port (ecml::RequestHandler::port(request));

    //metkit::MarsRequest mr (verb(request));
    //ecml::convertToMarsRequest<metkit::MarsRequest> (request, mr);

    metkit::MarsRequest mr(metkit::MarsRequest::parse(r));

    std::vector<std::string> hostVector;
    mr.getValues("database", hostVector);

    if (hostVector.size() != 1) {
        throw BadValue("Must specify database= value explicitly", Here());
    }

    std::string host(hostVector[0]);
    long port(9000);

    return new metkit::MarsRequestHandle(mr, new metkit::DHSProtocol(host, host, port));
}

} // namespace odb
