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
#include <algorithm>
#include <string>

#include "eckit/io/FileHandle.h"

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core/ExecutionContext.h"
#include "ecml/data/DataHandleFactory.h"

#include "ArchiveHandler.h"

#include "odb_api/FastODA2Request.h"
#include "odb_api/Archiver.h"

using namespace std;
using namespace ecml;

namespace odb {

const char * cfg_ = "let, CLASS=class, DATE=andate, TIME=antime, TYPE=type, OBSGROUP=groupid, REPORTYPE=reportype, STREAM=stream, EXPVER=expver";

ArchiveHandler::ArchiveHandler(const string& name) : RequestHandler(name) {}

Values ArchiveHandler::handle(ExecutionContext& context)
{
    const string host (database(context));
    const string protocol (host == "local" ? "local://" : "mars://");

    vector<string> sources (context.getValueAsList("source"));
    if (! sources.size())
        throw eckit::UserError("You must specify file(s) to be archived using the SOURCE keyword");

    Values r(0);
    List list(r);
    for (size_t i(0); i < sources.size(); ++i)
    {
        const string source (sources[i]);

        Request generatedRequest (generateRequest(source));

        generatedRequest->value("database", host);
        generatedRequest->value("odbpathnameschema", context.getValueAsList("odbpathnameschema")[0]);
        generatedRequest->value("odbserverroots", context.getValueAsList("odbserverroots")[0]);
        generatedRequest->value("source", source);

        ASSERT(generatedRequest->text() == "RETRIEVE");
        generatedRequest->text("ARCHIVE");

        checkRequestMatchesFilesMetaData(context, generatedRequest);
 
        eckit::Log::info() << "Request generated for file " << source << ":" << endl
                    << generatedRequest << endl;

        archive(source, host, generatedRequest, protocol);

        generatedRequest->text("RETRIEVE");
        list.append(protocol + generatedRequest->str());
    }

    return r;
}

void ArchiveHandler::checkRequestMatchesFilesMetaData(const ExecutionContext& context, const Request generatedRequest)
{
    // TODO
    /*
    for (Request::const_iterator it(generatedRequest.begin()); it != generatedRequest.end(); ++it)
    {
        string key (it->first);
        Values values (it->second);

        Log::info() << "check if request matches files metadata: key=" << key << " value=" << values << endl;

        if (request.find(key) == request.end() || ! request.at(key).size())
        {
            string message("keyword '" + key + "' was not set in user's request");
            //throw UserError(message);
            Log::warning() << message << endl;
        }
        else
        {
            // TODO: check values in range
        }
    }
    */
}

Request ArchiveHandler::generateRequest(const string& source)
{
    odb::FastODA2Request<odb::ODA2RequestClientTraits> o2r;
    o2r.parseConfig(cfg_);

    eckit::OffsetList offsets;
    eckit::LengthList lengths;
    std::vector<odb::ODAHandle*> handles;
    bool rc (o2r.scanFile(source, offsets, lengths, handles));
    for (size_t i (0); i < handles.size(); ++i)
        delete handles[i];
    handles.clear();
    if (! rc)
        throw eckit::UserError(string("Cannot archive file ") + source);
    eckit::Log::info() << "File " << source << " has " << o2r.rowsNumber() << " rows." << endl;

    ASSERT(lengths.size() && lengths.size() == offsets.size());

    string r ( o2r.genRequest() );
    std::replace(r.begin(), r.end(), '\n', ' ');

    r.erase(std::remove(r.begin(), r.end(), ' '), r.end());
    
    r = "RETRIEVE," + r;
    Request requests (RequestParser::parse(r));
    //ASSERT(requests.size() == 1);
    //return requests.front();
    ASSERT(requests->tag() == "_requests");

    //requests->showGraph(string("generateRequest: ") + requests->str() );
    
    return requests->value();
}

void ArchiveHandler::archive(const eckit::PathName& source, const string& host, const Request request, const string& protocol)
{
    eckit::Log::info() << "ARCHIVE " << source << " on " << host << endl;
    eckit::Log::info() << "ARCHIVE request: " << request << endl;

    eckit::FileHandle input(source);

    stringstream ss;
    ss << protocol << request;
    auto_ptr<eckit::DataHandle> mars (DataHandleFactory::openForWrite(ss.str(), eckit::Length(Archiver::fileSize(source))));

    input.saveInto(*mars);
}

} // namespace odb 

