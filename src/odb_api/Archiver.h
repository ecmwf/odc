/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Archiver.h
// Piotr Kuchta - ECMWF December 2015

#ifndef odb_api_Archiver_H
#define odb_api_Archiver_H

#include <fstream>

class Archiver {
public:

    static void archive(const eckit::PathName& path, 
                        const std::string& schema, 
                        const std::string& keywordsConfig,
                        const std::string& odbServerArchiveRoot);

    static void archive(eckit::MultiHandle&,
                        const std::vector<std::string>&,
                        const std::map<std::string,std::vector<std::string> >&);

    static std::ifstream::pos_type fileSize(const eckit::PathName&);
    static void createDirectories(const eckit::PathName&);
};

#endif
