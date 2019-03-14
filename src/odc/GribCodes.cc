/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file GribCodesBase.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/config/Resource.h"
#include "eckit/eckit.h"
#include "eckit/utils/StringTools.h"
#include "odc/GribCodes.h"
#include "odc/StringTool.h"

using namespace std;
using namespace eckit;

namespace odc {


ClassCodes* GribCodes::classCodes_ = 0; //ClassCodes();
TypeCodes* GribCodes::typeCodes_ = 0; //TypeCodes();
StreamCodes* GribCodes::streamCodes_ = 0; //StreamCodes();
GroupCodes* GribCodes::obsgroupCodes_ = 0;//  GroupCodes();

void GribCodes::load()
{
	if (! classCodes_) classCodes_ = new ClassCodes();
	if (! typeCodes_) typeCodes_ = new TypeCodes();
	if (! streamCodes_) streamCodes_ = new StreamCodes();
	if (! obsgroupCodes_) obsgroupCodes_ =  new GroupCodes();
}

std::string GribCodes::numeric(const std::string& keyword, const std::string& alphanumeric)
{
	load();
	const std::string kw = StringTools::upper(keyword);
	if (kw == "TYPE")
	{
		// HACK:
		if (alphanumeric == "OFB") return "263";
		if (alphanumeric == "MFB") return "262";

		return typeCodes_->numeric(alphanumeric);
	}
	if (kw == "CLASS") return classCodes_->numeric(alphanumeric);
	if (kw == "STREAM") return streamCodes_->numeric(alphanumeric);
	if (kw == "OBSGROUP") return obsgroupCodes_->numeric(alphanumeric);

	throw eckit::UserError(std::string("'") + keyword + "' is not a known GRIB keyword");
	return "Don't want the compiler to warn me about non-void function not returning anything";
}

std::string GribCodes::alphanumeric(const std::string& keyword, const std::string& numeric)
{
    load();
    const std::string kw (StringTools::upper(keyword));
    if (kw == "TYPE") 
    {
        // HACK:
        if (numeric == "262") return "MFB";
        if (numeric == "263") return "OFB";

        return typeCodes_->alphanumeric(numeric);
    }
    if (kw == "CLASS") return classCodes_->alphanumeric(numeric);
    if (kw == "STREAM") return streamCodes_->alphanumeric(numeric);
    if (kw == "OBSGROUP") return obsgroupCodes_->alphanumeric(numeric);

    throw eckit::UserError(std::string("'") + keyword + "' is not a known GRIB keyword");
    return "Don't want the compiler to warn me about non-void function not returning anything";
}

GribCodesBase::GribCodesBase(const PathName& fileName)
: configFileName_( std::string(Resource<std::string>("$odc_CODES", "/usr/local/apps/odc/codes/")) + "/" + fileName),
  fieldDelimiter_(" \t"),
  mapsLoaded_(false),
  numericIndex_(0),
  alphanumericIndex_(1)
{
    Log::debug() << "GribCodesBase::GribCodesBase: configFileName_:" << configFileName_ << std::endl;
    readConfig(configFileName_);
}

GribCodesBase::GribCodesBase(const PathName& fileName, const std::string& fieldDelimiter, size_t numericIndex, size_t alphanumericIndex)
: configFileName_(std::string(Resource<std::string>("$odc_CODES", "/usr/local/apps/odc/codes/")) + "/" + fileName),
  fieldDelimiter_(fieldDelimiter),
  mapsLoaded_(false),
  numericIndex_(numericIndex),
  alphanumericIndex_(alphanumericIndex)
{
    Log::debug() << "GribCodesBase::GribCodesBase: configFileName_:" << configFileName_ << std::endl;
    readConfig(configFileName_);
}

void GribCodesBase::readConfig(const PathName& fileName)
{
	Log::debug() << "GribCodesBase::readConfig(" << fileName << ")" << std::endl;
	numeric2alpha_.clear();
	alpha2numeric_.clear();

	std::vector<std::string> lines (StringTool::readLines(fileName));
	for (size_t i = 0; i < lines.size(); ++i)
	{
		std::vector<std::string> words = StringTools::split(fieldDelimiter_, lines[i]);
		if (words.size() >= 2)
		{
			std::string num (StringTools::trim(words[numericIndex_]));
			std::string alpha (StringTools::trim(words[alphanumericIndex_]));
			numeric2alpha_[num] = alpha;
			alpha2numeric_[alpha] = num;
			Log::debug() << "GribCodesBase::readConfig: num='" << num << "' alpha='" << alpha << "'" << std::endl;
		}
	}

	mapsLoaded_ = true;
}

std::string GribCodesBase::numeric(const std::string& alphanumeric)
{
	//if (!mapsLoaded_) readConfig(configFileName_);
	if (alpha2numeric_.find(alphanumeric) == alpha2numeric_.end())
		throw eckit::UserError(std::string("Alphanumeric code '") + alphanumeric + "' not found in '" + configFileName_ + "'");
	return alpha2numeric_[alphanumeric];
}

//TODO:
//int numericAsInt(const std::string& alphanumeric);

std::string GribCodesBase::alphanumeric(const std::string& numeric) {
	//if (!mapsLoaded_) readConfig(configFileName_);
	if (numeric2alpha_.find(numeric) == numeric2alpha_.end())
		throw eckit::UserError(std::string("Numeric code ") + numeric + " not found in '" + configFileName_ + "'");
	return numeric2alpha_[numeric];
}

// TODO:
//std::string alphanumeric(int);

} // namespace odc 

