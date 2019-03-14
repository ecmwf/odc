/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/StringTools.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MultiHandle.h"

#include "odc/FileMapper.h"
#include "odc/Odb2Hub.h"
#include "odc/Archiver.h"
#include "odc/FileCollector.h"
#include "odc/RequestUtils.h"

#include <stdio.h>
#include <fstream>

using namespace std;
using namespace eckit;

std::ifstream::pos_type Archiver::fileSize(const PathName& path)
{
    std::ifstream in(path.asString().c_str(), std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

void Archiver::createDirectories(const PathName& path)
{
    vector<string> parts (StringTools::split("/", path));
    parts.pop_back();
    PathName directory ((string(path)[0] == '/' ? "/" : "") + StringTools::join("/", parts));
    directory.mkdir();
}

/// Archive single file. File given by path will be moved to target location
/// according to schema, etc.
void Archiver::archive(const eckit::PathName& path, 
                          const std::string& dirtySchema, 
                          const std::string& dirtyKeywordsConfig,
                          const std::string& dirtyOdbServerArchiveRoot)
{
    const std::string schema (StringTools::unQuote(dirtySchema));
    const std::string keywordsConfig (StringTools::unQuote(dirtyKeywordsConfig));
    const std::string odbServerArchiveRoot (StringTools::unQuote(dirtyOdbServerArchiveRoot));

    PathName targetPath (odbServerArchiveRoot + "/" + Odb2Hub::getPath(schema, path, keywordsConfig));

    createDirectories(targetPath);

    unsigned long long transferedFileSize (fileSize(path));
    if (rename(string(path).c_str(), string(targetPath).c_str()))
        throw SeriousBug(string(string("Unsuccessfull rename of ") + path + " to " + string(targetPath)).c_str());
    else 
    {
        unsigned long long targetFileSize (fileSize(targetPath));
        if (transferedFileSize != targetFileSize)
        {
            stringstream ss;
            ss << "Transfered file " << path << " size is " << transferedFileSize 
                << ", after renaming to " << targetPath << " the size is " << targetFileSize
                <<  endl;
            throw Exception(ss.str());
        }
        Log::info() << "File " << path << " successfully renamed to " << string(targetPath) << endl;
    }
}

void Archiver::archive(eckit::MultiHandle& h,
                       const std::vector<std::string>& keywords,
                       const std::map<std::string,std::vector<std::string> >& request)
{
    odc::RequestDict r(odc::unquoteRequestValues(request));

    vector<string> sources (r["source"]);
    if (sources.size() == 0) throw UserError("ARCHIVE missing SOURCE");
    if (sources.size() > 1) throw UserError("ARCHIVE supports single SOURCE currently");

    const string source (sources[0]);
    const string schema (r["odbpathnameschema"][0]);
    const string odbServerArchiveRoot (r["odbserverroots"][0]);
    // TODO: this is a resource in dhshome/etc/config/local
    const string odbServerKeywordsConfig (
    "let,class=class,stream=stream,expver=expver,date=andate,time=antime,type=type,obsgroup=groupid,reportype=reportype");

    PathName targetPath (FileCollector::expandTilde(odbServerArchiveRoot) + "/" + Odb2Hub::getPath(schema, source, odbServerKeywordsConfig));

    Log::info() << "targetPath: " << targetPath << endl;

    createDirectories(targetPath);
    eckit::DataHandle * fh (new eckit::FileHandle(targetPath));

    if (! eckit::PathName(source).exists())
        throw UserError(string("ARCHIVE: file '") + source + "' doesn't exist." );

    h += fh;
    h += Length(fileSize(source));

}
