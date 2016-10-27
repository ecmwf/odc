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
#include "eckit/io/TCPHandle.h"

#include "eckit/parser/StringTools.h"

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core/RequestHandler.h"

#include "metkit/MarsRequestHandle.h"
#include "metkit/DHSProtocol.h"

#include "ecml/data/HttpHandle.h"
#include "ecml/data/HttpHandleFactory.h"

using namespace eckit;
using namespace std;

namespace ecml {

HttpHandleFactory::HttpHandleFactory()
: DataHandleFactory("http")
{}

eckit::DataHandle* HttpHandleFactory::makeHandle(const std::string& r) const
{
    return new HttpHandle(r);
}

} // namespace ecml
