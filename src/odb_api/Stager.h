/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Stager.h
// Piotr Kuchta - ECMWF December 2015

#ifndef odb_api_Stager_H
#define odb_api_Stager_H

#include "odb_api/Partitions.h"
#include "odb_api/FileMapper.h"

class Stager {
public:

    static void stage(eckit::MultiHandle&                                    output,
                      const std::vector<std::string>&                        keywords,
                      const std::map<std::string,std::vector<std::string> >& request);

private:

    static void prepareMapper(FileMapper&                                            mapper,
                              const std::vector<std::string>&                        keywords,
                              const std::map<std::string,std::vector<std::string> >& request);

    static void createIndices(const std::vector<eckit::PathName>&);
    static void sendPartitionsInfo(eckit::MultiHandle&, const odb::Partitions&);
    static size_t numberOfPartitions(const std::map<std::string,std::vector<std::string> >&);
    static std::vector<eckit::PathName> writePartitionsToFiles (const odb::Partitions&, const std::string& pathNamePrefix, const std::string& fileListPath);
};

#endif
