/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef odb_api_ArchiveHandler_H
#define odb_api_ArchiveHandler_H

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/RequestHandler.h"

namespace odb {

class ArchiveHandler : public eckit::RequestHandler {
public:
    ArchiveHandler(const std::string&);

    virtual eckit::Values handle(eckit::ExecutionContext&);

    static eckit::Request generateRequest(const std::string&);

private:
    void archive(const eckit::PathName& source, const std::string& host, const eckit::Request request);
    static void checkRequestMatchesFilesMetaData(const eckit::ExecutionContext&, const eckit::Request);
};

} // namespace odb

#endif
