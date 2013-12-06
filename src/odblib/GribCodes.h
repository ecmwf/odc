/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file GribCodes.h
///
/// @author Piotr Kuchta, ECMWF, April 2011

#ifndef GribCodes_H
#define GribCodes_H

//#include <map>
//#include <set>
//#include <vector>

#include "eckit/filesystem/PathName.h"
//#include "eckit/utils/Tokenizer.h"

//#include "odblib/StringTool.h"


namespace eckit { class PathName; }


namespace odb {

class GribCodesBase {
public:
	GribCodesBase(const eckit::PathName&);
	GribCodesBase(const eckit::PathName&, const std::string& fieldDelimiter);

	virtual void readConfig(const eckit::PathName& fileName);

	virtual std::string numeric(const std::string& alphanumeric);
	virtual std::string alphanumeric(const std::string& numeric);
private:
	eckit::PathName configFileName_;
	std::string fieldDelimiter_;
	bool mapsLoaded_;
	std::map<std::string,std::string> numeric2alpha_;
	std::map<std::string,std::string> alpha2numeric_;
};


class ClassCodes : public GribCodesBase {
public:
	ClassCodes() : GribCodesBase("class.table") {}
};

class TypeCodes : public GribCodesBase {
public:
	TypeCodes() : GribCodesBase("type.table") {}
};

class StreamCodes : public GribCodesBase {
public:
	StreamCodes() : GribCodesBase("stream.table") {}
};

class GroupCodes : public GribCodesBase {
public:
	GroupCodes() : GribCodesBase("obsgroups.txt", ";") {}
};

class GribCodes {
public:
	static std::string numeric(const std::string& keyword, const std::string& alphanumeric);
	static std::string alphanumeric(const std::string& keyword, const std::string& numeric);

private:
	static void load();
	static ClassCodes* classCodes_;
	static TypeCodes* typeCodes_;
	static StreamCodes* streamCodes_;
	static GroupCodes* obsgroupCodes_;
};


} // namespace odb 

#endif

