/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FileMapper.h
/// Piotr Kuchta - ECMWF Feb 12

#ifndef FileMapper_H
#define FileMapper_H

#include "eckit/memory/NonCopyable.h"

class FileMapper : private eckit::NonCopyable {
public:
    
	FileMapper(const std::string& pathNameSchema);
	~FileMapper();
	
	void addRoot(const std::string&);
	void addRoots(const std::vector<std::string>&);
	void checkRoots() const;

	std::string encodeRelative(const std::map<std::string,std::string>& values) const;
	std::vector<std::string> encode(const std::map<std::string,std::string>& values) const;

	std::vector<std::string> keywords() const;

protected:
    
	void parsePathNameSchema(const std::string& pathNameSchema);

private:
	std::vector<std::string> placeholders_;
	std::vector<std::string> separators_;

	std::vector<std::string> roots_;

    std::string patchTime(const std::string&) const;
};

#endif
