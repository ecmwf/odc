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

#ifndef ArchiveHandler_H
#define ArchiveHandler_H

#include "experimental/ecml/parser/Request.h"
#include "experimental/ecml/RequestHandler.h"

class ArchiveHandler : public eckit::RequestHandler {
public:
    ArchiveHandler(const std::string&);

    virtual eckit::Values handle(eckit::ExecutionContext&);

    static eckit::Request generateRequest(const std::string&);

private:
    void archive(const eckit::PathName& source, const std::string& host, const eckit::Request request);
    static void checkRequestMatchesFilesMetaData(const eckit::ExecutionContext&, const eckit::Request);
};

#endif
