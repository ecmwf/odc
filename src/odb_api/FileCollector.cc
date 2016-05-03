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

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Timer.h"
#include "eckit/config/Resource.h"

#include "odb_api/FileCollector.h"
#include "odb_api/FileMapper.h"

using namespace eckit;
using namespace std;

FileCollector::FileCollector(const FileMapper& fm, MultiHandle& mh)
: mapper_(fm),
  multiHandle_(mh),
  allFound_()
{}

string FileCollector::expandTilde(const string& s) 
{
    string dhsHome (::getenv("DHSHOME") ? ::getenv("DHSHOME") 
                    : ::getenv("TEST_DHSHOME") ? ::getenv("TEST_DHSHOME")
                    : "");

    if (! dhsHome.size())
        throw UserError("expandTilde: DHSHOME or TEST_DHSHOME must be set");

    if (s.size() && s[0] == '~')
        return dhsHome + "/" + s.substr(1);

    return s;
}

FileCollector::~FileCollector() {}

string FileCollector::prestageScriptPath(const std::map<std::string,std::vector<std::string> >& r) const
{
    string prestageScript;
    if (r.find("odbServerPrestageScript") != r.end())
    {
        vector<string> odbServerPrestageScripts (r.at("odbServerPrestageScript"));
        if (odbServerPrestageScripts.size() > 1)
            throw UserError("odbServerPrestageScript must be one value at most");
        if (odbServerPrestageScripts.size() == 1)
            prestageScript = odbServerPrestageScripts[0];
    }
    if (prestageScript.size() == 0)
        prestageScript = string(Resource<string>("$ODB_SERVER_PRESTAGE_SCRIPT", ""));
    if (prestageScript.size() == 0)
        prestageScript = string(Resource<string>("odbServerPrestageScript", ""));

    return expandTilde(prestageScript);
}

void FileCollector::prestage(const std::map<std::string,std::vector<std::string> >& r, const string& fileName)
{
    static string prestageScript (prestageScriptPath(r));
    if (prestageScript.size() == 0)
        return;

    Timer timer("Prestage " + fileName);
    stringstream cmd;
    cmd << prestageScript << " " << fileName;
    Log::debug() << "Calling prestage script: '" << cmd.str() << "'" << endl;
    int rc (::system(cmd.str().c_str()));
    if (rc != 0) {
        stringstream ss;
        ss << "prestage command '" << cmd.str() << "' failed (return code: " << rc << ")";
        Log::error() << ss.str() << endl;
        throw SeriousBug(ss.str());
    }
}

std::vector<std::string> FileCollector::foundFiles() const { return allFound_; }

std::vector<eckit::PathName> FileCollector::foundFilesAsPathNames() const 
{ 
    std::vector<eckit::PathName> r;
    for (size_t i(0); i < allFound_.size(); ++i)
        r.push_back(allFound_[i]);
    return r;
}

void FileCollector::collectFile(const std::map<std::string,std::vector<std::string> >& r, const map<string,string>& values)
{
    const string relativePath (mapper_.encodeRelative(values));

    vector<string> foundFiles, files (mapper_.encode(values));
    ASSERT(files.size() > 0);
    for (size_t i (0); i < files.size(); ++i)
    {
        const string& p (expandTilde(files[i]));
        prestage(r, p);
        if (PathName(p).exists())
        {
            Log::info() << "FileCollector::collectFile: FOUND " << p << endl;
            foundFiles.push_back(p);
        } else {
            Log::info() << "FileCollector::collectFile: NOT found " << p << endl;
        }
    }

    if (foundFiles.size() > 1)
    {
        stringstream ss;
        ss << "File " << relativePath << " found in more than one root directory: " << foundFiles;
        throw UserError(ss.str());
    }

    if (foundFiles.size() == 1)
    {
        allFound_.push_back(foundFiles[0]);
        multiHandle_ += PathName(foundFiles[0]).fileHandle();
    }
}

void FileCollector::findFiles(const vector<string>& keywords, const map<string, vector<string> >& request)
{
    product(0, keywords, request, map<string,string>());
}

void FileCollector::product(size_t k, 
                            const vector<string>& keywords, 
                            const map<string, vector<string> >& request, 
                            const map<string,string>& combination)
{
	if (k == keywords.size())
		return collectFile(request, combination);

	const string& keyword (keywords[k]);
	const vector<string>& values (request.find(keyword)->second);
	for (size_t i (0); i < values.size(); ++i)
	{
		map<string,string> newCombination(combination);
		newCombination[keyword] = values[i];
		product(k + 1, keywords, request, newCombination);
	}
}

