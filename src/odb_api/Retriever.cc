/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <ctype.h>
#include <fstream>
#include <algorithm>    // std::find

#include "eckit/config/Resource.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/PartFileHandle.h"

#include "ecml/core/Interpreter.h"
#include "ecml/core/ExecutionContext.h"

#include "odb_api/FileCollector.h"
#include "odb_api/FileMapper.h"
#include "odb_api/ODBModule.h"
#include "odb_api/InMemoryDataHandle.h"
#include "odb_api/Partition.h"
#include "odb_api/Writer.h"
#include "odb_api/Reader.h"
#include "odb_api/Retriever.h"

using namespace std;
using namespace eckit;
using namespace odb;
using namespace odb::tool;

void Retriever::checkKeywordsHaveValues(const std::map<std::string,std::vector<std::string> >& request, const vector<string>& keywords)
{
    typedef std::map<std::string,std::vector<std::string> > ms;
    ms r;
    for (ms::const_iterator it (request.begin()); it != request.end(); ++it)
        r[eckit::StringTools::lower(it->first)] = it->second;

    for (size_t i (0); i < keywords.size(); ++i)
    {
        const vector<string>& values ( r[ keywords[i] ]);
        if (values.size() < 1)
            throw eckit::UserError( "At least one value required for keyword '" + keywords[i] + "'");
        Log::info()  << ":: - " << keywords[i] << " " << values << endl;
    }
}

void Retriever::retrieve(MultiHandle&                                           output, 
                         const std::vector<std::string>&                        keywords, 
                         const std::map<std::string,std::vector<std::string> >& r)
{
    std::map<std::string,std::vector<std::string> > request(r);

    if (r.count("odbpathnameschema") == 0)
        throw UserError("RETRIEVE: odbpathnameschema not set");

    FileMapper mapper(r.at("odbpathnameschema")[0]);

    vector<string> odbServerRoots (eckit::StringTools::split(":", r.at("odbserverroots")[0]));
    for (size_t i(0); i < odbServerRoots.size(); ++i)
        odbServerRoots[i] = FileCollector::expandTilde(odbServerRoots[i]);
    mapper.addRoots(odbServerRoots);
    mapper.checkRoots();

    checkKeywordsHaveValues(request, keywords);

    const vector<string> partitionNumbers ( request["part_number"] );

    if (partitionNumbers.size())
    {
        const string partitionsInfo (FileCollector::expandTilde(request["partitionsinfo"][0]));

        vector<size_t> parts;
        for (size_t i(0); i < partitionNumbers.size(); ++i)
            parts.push_back( atoll(partitionNumbers[i].c_str()) );

        sendPartitions (output, PathName(partitionsInfo), parts);
        //sendSavedPartitions (output, PathName(partitionsInfo + ".files"), parts);
    } 
    else
    {
#ifdef HAVE_ODB_API_SERVER_SIDE
        // Check server_side 
        vector<string> serverSide (request ["server_side"]);
        if (serverSide.size()) 
        {
            MultiHandle mh;
            FileCollector fileCollector (mapper, mh);
            fileCollector.findFiles(keywords, request);

            if(mh.estimate() == Length(0)) Log::userWarning() << "Data not found" << endl;
            handleServerSide(output, fileCollector, serverSide);
        } 
        else 
        {
            Log::debug() << "No server side processing" << endl;
            FileCollector fileCollector (mapper, output);
            fileCollector.findFiles(keywords, request);

            if(output.estimate() == Length(0)) Log::userWarning() << "Data not found" << endl;
        }
#else
        Log::debug() << "Server side processing disabled, not checking SERVER_SIDE" << endl;
#endif
    }
}

void Retriever::sendSavedPartitions(MultiHandle& output, const PathName& savedPartitionsListFile, const std::vector<size_t>& partitionNumbers)
{
    vector<std::string> files (StringTool::readLines(savedPartitionsListFile, true));
    for (size_t i (0); i < files.size(); ++i)
    {
        if (std::find(partitionNumbers.begin(), partitionNumbers.end(), i) != partitionNumbers.end())
            output += PathName(files[i]).fileHandle();
    }
}

void Retriever::sendPartitions(MultiHandle& output, const PathName& partitionsInfo, const std::vector<size_t>& partitionNumbers)
{
    for (size_t i(0); i < partitionNumbers.size(); ++i)
    {
        Partition partition (partitionsInfo, partitionNumbers[i]);

        InMemoryDataHandle* dh (new InMemoryDataHandle);
        dh->openForWrite(0);

        partition.write(*dh);

        output += dh;
    }
}

void Retriever::handleServerSide(MultiHandle& output, const FileCollector& fileCollector, const vector<string>& serverSide)
{
    if (serverSide.size() > 1)
        throw UserError("Currently SERVER_SIDE can have only one value");

    const string foundFiles ( StringTools::join("\"/\"", fileCollector.foundFiles()) );
    string code (string() 
        + "apply, closure=" + serverSide[0] + ","
          "       args=(let,source=\"" + foundFiles + "\")");

    Log::info() << "server_side: '" << endl << code << endl << "'" << endl;

    ecml::ExecutionContext context;
    odb::ODBModule odbModule;
    context.import(odbModule);

    // server_side should return a list of files
    ecml::Values result (context.execute(code));
    Log::info() << "server_side result: " << result << endl;

    for (ecml::Cell* v (result); v; v = v->rest())
    {
        ASSERT(v->value());
        string fileName (v->value()->text());
        Log::info() << "SERVER_SIDE: adding '" << fileName << "' to output" << endl;
        output += PathName(fileName).fileHandle();
    }
}
