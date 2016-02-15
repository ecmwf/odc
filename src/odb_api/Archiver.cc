/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/StringTools.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"

#include "odb_api/odb_api.h"
#include "odb_api/StringTool.h"
#include "odb_api/FileMapper.h"
#include "odb_api/Odb2Hub.h"
#include "odb_api/Archiver.h"

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
                          const std::string& schema, 
                          const std::string& keywordsConfig,
                          const std::string& odbServerArchiveRoot)
{
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

