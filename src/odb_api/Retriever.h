/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Retriever.h
// Piotr Kuchta - ECMWF December 2015

#ifndef odb_api_Retriever_H
#define odb_api_Retriever_H

class FileMapper;
class FileCollector;

namespace eckit { class MultiHandle; class ExecutionContext; }

class Retriever {
public:
    static void checkKeywordsHaveValues (const std::map<std::string,std::vector<std::string> >& request, const std::vector<std::string>& keywords);

    static void retrieve (eckit::MultiHandle&                                    output, 
                          const std::vector<std::string>&                        keywords, 
                          const std::map<std::string,std::vector<std::string> >& request);
private:
    static void sendPartitions(eckit::MultiHandle& output, const eckit::PathName& partitionsInfo, const std::vector<size_t>& partitionNumbers);
    static void sendSavedPartitions(eckit::MultiHandle& output, const eckit::PathName& savedPartitionsListFile, const std::vector<size_t>& partitionNumbers);
    static void handleServerSide(eckit::MultiHandle&, const FileCollector&, const std::vector<std::string>&);
};

#endif
