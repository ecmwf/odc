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

#include <map>
#include <set>
#include <vector>

#include "eclib/Tokenizer.h"

#include "odblib/StringTool.h"

using namespace std;

namespace eclib { class PathName; }


namespace odb {

class GribCodesBase {
public:
	GribCodesBase(const eclib::PathName&);
	GribCodesBase(const eclib::PathName&, const string& fieldDelimiter);

	virtual void readConfig(const eclib::PathName& fileName);

	virtual string numeric(const string& alphanumeric);
	virtual string alphanumeric(const string& numeric);
private:
	eclib::PathName configFileName_;
	string fieldDelimiter_;
	bool mapsLoaded_;
	map<string,string> numeric2alpha_;
	map<string,string> alpha2numeric_;
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
	static string numeric(const string& keyword, const string& alphanumeric);
	static string alphanumeric(const string& keyword, const string& numeric);

private:
	static void load();
	static ClassCodes* classCodes_;
	static TypeCodes* typeCodes_;
	static StreamCodes* streamCodes_;
	static GroupCodes* obsgroupCodes_;
};


} // namespace odb 

#endif

