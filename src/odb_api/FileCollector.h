/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FileCollector.h
/// Piotr Kuchta - ECMWF Feb 12

#ifndef FileCollector_H
#define FileCollector_H

#include "eckit/memory/NonCopyable.h"
#include "eckit/io/MultiHandle.h"

class FileMapper;

class FileCollector : private eckit::NonCopyable {
public:
    
	FileCollector(const FileMapper&, eckit::MultiHandle&);
	virtual ~FileCollector();

    void findFiles(const std::vector<std::string>&, const std::map<std::string,std::vector<std::string> >&);

    void prestage(const std::map<std::string,std::vector<std::string> >&, const std::string& fileName);

    std::vector<std::string> foundFiles() const;
    std::vector<eckit::PathName> foundFilesAsPathNames() const;

    static std::string expandTilde(const std::string& s);

protected:
	virtual void collectFile(const std::map<std::string,std::vector<std::string> >&, const std::map<std::string,std::string>& values);

private:
	void product(size_t k, const std::vector<std::string>& keywords, const std::map<std::string,std::vector<std::string> >&, const std::map<std::string,std::string>&);
    std::string prestageScriptPath(const std::map<std::string, std::vector<std::string> >&r) const;

    const FileMapper& mapper_;
	eckit::MultiHandle& multiHandle_;
    std::vector<std::string> allFound_;
};

#endif
