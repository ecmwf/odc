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

#include "odb_api/FileCollector.h"
#include "odb_api/FileMapper.h"
#include "odb_api/InMemoryDataHandle.h"
#include "odb_api/Partition.h"
#include "odb_api/Writer.h"
#include "odb_api/Reader.h"
#include "odb_api/Retriever.h"

using namespace std;
using namespace eckit;
using namespace odc;
using namespace odc::tool;

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
    std::map<std::string,std::vector<std::string> > request(unquoteValues(r));

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
        // Check server_side 
        vector<string> serverSide (request ["server_side"]);
        if (serverSide.size()) 
        {
            Log::error() << "ODB API server side no longer exists" << endl;
            throw UserError("ODB API server side no longer exists");
        } 
        else 
        {
            Log::debug() << "No server side processing" << endl;
            FileCollector fileCollector (mapper, output);
            fileCollector.findFiles(keywords, request);

            if(output.estimate() == Length(0)) 
                Log::userWarning() << "Data not found" << endl;
        }
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


std::map<std::string,std::vector<std::string> > Retriever::unquoteValues(const std::map<std::string,std::vector<std::string> >& request)
{
    std::map<std::string,std::vector<std::string> > r;

    for ( std::map<std::string,std::vector<std::string> >::const_iterator it (request.begin()); it != request.end(); ++it)
    {
        const std::string& key (it->first);
        const std::vector<std::string>& values (it->second);
        std::vector<std::string> vs;
        for (size_t i(0); i < values.size(); ++i)
            vs.push_back(StringTool::unQuote(values[i]));
        r[key] = vs;
    }

    return r;
}
