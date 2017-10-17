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

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/types/Types.h"
#include "eckit/parser/StringTools.h"
#include "eckit/filesystem/PathName.h"

#include "odb_api/FileMapper.h"
#include "odb_api/FileCollector.h"

using namespace eckit;
using namespace std;

typedef StringTools S;

static Mutex local_mutex;

FileMapper::FileMapper(const string& pathNameSchema)
{
	parsePathNameSchema(pathNameSchema);
}

FileMapper::~FileMapper() {}

// Order of keywords: /tmp/p4/mars/client/dev/grib_api/src/hypercube.c

void FileMapper::checkRoots() const
{
    AutoLock<Mutex> lock(local_mutex);
    ASSERT("FileMapper: roots_ not set" && roots_.size());

    bool atLeastOneRootExists (false);

    for (size_t i (0); i < roots_.size(); ++i)
    {
        const string& p (roots_[i]);
        bool exists (PathName(roots_[i]).exists());

        Log::info() << "checkRoots: " << i << ": " << roots_[i]
            << " " << exists << std::endl;

        if (exists)
            atLeastOneRootExists = true;
    }
    if (! atLeastOneRootExists)
    {
        stringstream msg;
        msg << "No directory specified in odbServerRoots exists, checked: " << roots_[0];
        for (size_t i(1); i < roots_.size(); ++i)
            msg << ":" << roots_[i];

        throw UserError(msg.str());
    }
}

void FileMapper::parsePathNameSchema(const std::string& pathNameSchema)
{
    AutoLock<Mutex> lock(local_mutex);

    Log::debug() << "pathNameSchema: " << pathNameSchema << std::endl;

    placeholders_.clear();
    separators_.clear();
    const string& s (pathNameSchema);
    for (size_t i (0); i < s.size(); )
    {
        size_t begin (s.find('{', i));
        if (begin == std::string::npos)
        {
            separators_.push_back(s.substr(i));
            break;
        }
        size_t end (s.find('}', begin));
        ASSERT(end != std::string::npos);

        separators_.push_back(s.substr(i, begin - i));
        placeholders_.push_back(s.substr(begin + 1, end - begin - 1));

        i = end + 1;
    }

    // TODO: keywords_ needs to be sorted as in //depot/mars/client/dev/grib_api/src/hypercube.c
    //keywords_ = placeholders_;

    Log::debug() << "pathNameSchema: separators_=" << separators_ << std::endl;
    Log::debug() << "pathNameSchema: placeholders_ =" << placeholders_ << std::endl;
}

std::vector<std::string> FileMapper::keywords() const { return placeholders_; }

void FileMapper::addRoot(const std::string& p)
{
	roots_.push_back(FileCollector::expandTilde(p));
}

void FileMapper::addRoots(const std::vector<std::string>& roots)
{
    for (size_t i (0); i < roots.size(); ++i)
        addRoot(roots[i]);
}

string FileMapper::patchTime(const string& s) const
{
    ASSERT("Format of time" && s.size() != 3 && !(s.size() > 6));
    string r (s);

#ifdef ODB_SERVER_TIME_FORMAT_FOUR_DIGITS
    if (s.size() == 1) r = string("0") + s + "00";
    //                '60000' => '0600'
    if (s.size() == 5) r = string("0") + s.substr(0,3);
    //                '120000' => '1200'
    if (s.size() == 6) r = s.substr(0,4);

    ASSERT(r.size() == 4); // We want time as four digits, don't we....
#else
    if (s.size() == 1) r = string("0") + s;
    // HACK: for TIME '0600' => '06'
    if (s.size() == 4) r = s.substr(0,2);
    //                '60000' => '06'
    if (s.size() == 5) r = string("0") + s.substr(0,1);
    //                '120000' => '12'
    if (s.size() == 6) r = s.substr(0,2);

    ASSERT(r.size() == 2); // We want time as two digits, don't we....
#endif

    return r;
}

string FileMapper::encodeRelative(const std::map<std::string,std::string>& values) const
{
    ostream& L(Log::info());
    L << "FileMapper::encode: values:" << endl;
    for (map<string,string>::const_iterator it(values.begin()); it != values.end(); ++it)
        L << "    " <<  it->first << ":" << it->second << endl;

    stringstream r;
    size_t pi (0);
    for (size_t i (0); i < separators_.size(); ++i)
    {
        r << separators_[i];
        if (pi < placeholders_.size())
        {
            string placeholder (S::upper(placeholders_[pi++]));

            const map<string,string>::const_iterator end(values.end());

            if (values.find(placeholder) == end && values.find(S::lower(placeholder)) == end)
                throw UserError(string("Could not find value of '") + placeholder + "' in values suplied.");

            const map<string,string>::const_iterator it( values.find(placeholder) != end
                                                        ? values.find(placeholder)
                                                        : values.find(S::lower(placeholder)) );

            string value (it->second);
            string patchedValue
                // ((S::upper(placeholder) == "TIME" || S::upper(placeholder) == "ANTIME")
                ((S::upper(placeholder) == "ANTIME")
                ? patchTime(value)
                : value);
            if (value != patchedValue)
                L << "FileMapper::encodeRelative: value of '" << placeholder << "' was '" << value << "' changed to '" << patchedValue << "'" << endl;

            r << patchedValue;
        }
    }
    return r.str();
}

vector<string> FileMapper::encode(const std::map<std::string,std::string>& values) const
{
    string path (encodeRelative(values));
    vector<string> r;
    for (size_t i (0); i < roots_.size(); ++i)
        r.push_back(roots_[i] + '/' + path);
    return r;
}
