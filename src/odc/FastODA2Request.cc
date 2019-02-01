/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/parser/StringTools.h"
#include "eckit/parser/Tokenizer.h"
#include "odc/GribCodes.h"
#include "odc/core/TablesReader.h"
#include "odc/ODAHandle.h"

#include <algorithm>
#include <string>
#include <memory>
#include <ostream>

using namespace odc::core;

namespace odc {

template <typename T>
FastODA2Request<T>::FastODA2Request()
: firstMD_(0),
  rowsNumber_(0),
  mergeSimilarBlocks_(true)
{}

template <typename T>
void FastODA2Request<T>::parseConfig(const std::string& s)
{
    // We don't want to do ECML any more.
    // This is a bit of a hack to do something that works...

    eckit::Tokenizer tokenizer(",= ':\n", false);

    std::vector<std::string> tokens;
    tokenizer(s, tokens);


    // We support both an old ECML-style parsing (let, DATE=..., TIME=...) and a newer format
    // (DATE: ...\nTIME: ...\n)

    ASSERT(tokens.size() > 1);

    std::string vrb = tokens[0];
    std::transform(vrb.begin(), vrb.end(), vrb.begin(), ::tolower);
    size_t startIndex = 0;

    if (vrb == "let") {
        ASSERT((tokens.size() - 1) % 2 == 0);
        startIndex = 1;
    } else {
        ASSERT(tokens.size() % 2 == 0);
    }

    for (size_t i = startIndex; i < tokens.size(); i += 2) {
        eckit::Log::debug() << "parseConfig: " << tokens[i] << "=" << tokens[i+1] << std::endl;
        addColumn(tokens[i], tokens[i+1]);
    }
}

template <typename T>
void FastODA2Request<T>::addColumn(const std::string& keyword, const std::string& columnName)
{
	keywords_.push_back(keyword);
	columnNames_.push_back(columnName);
}

template <typename T>
bool FastODA2Request<T>::scanFile(const eckit::PathName& fileName)
{
	eckit::OffsetList offsets;
	eckit::LengthList lengths;
	std::vector<ODAHandle*> handles;

	bool r = scanFile(fileName, offsets, lengths, handles);

	for (size_t i = 0; i < handles.size(); ++i)
		delete handles[i];
	handles.clear();

	return r;
}

template <typename T>
bool FastODA2Request<T>::scanFile(const eckit::PathName& fileName, eckit::OffsetList& offsets, eckit::LengthList& lengths, std::vector<ODAHandle*>& handles)
{
    using eckit::Log;
    std::ostream& L (Log::debug());

	L << "Iterating over headers of '" << fileName << "'" <<  std::endl;
	
    inputFile_ = fileName;

    core::TablesReader reader(fileName);
    auto it = reader.begin();
    auto end = reader.end();

    std::unique_ptr<MetaData> currentMD(it->columns().clone());
    currentMD->resetStats();
	rowsNumber_ = currentMD->rowsNumber();

    values_ = std::vector<std::set<std::string> >(currentMD->size(), std::set<std::string>());
	unsigned long int mds = 0;	
	for ( ; it != end; ++it)
	{
        const MetaData& md (it->columns());
		++mds;

        eckit::Offset startOffset = it->startPosition();
        eckit::Offset endOffset = it->nextPosition();
		eckit::Length blockSize = endOffset - startOffset;

		if (!offsets.size() || !mergeSimilarBlocks_ || !currentMD->equalsIncludingConstants(md, columnNames_))
		{
			L << "FastODA2Request@" << this << "::scanFile: new handle for <" << startOffset << "," << endOffset << ">" << std::endl;

			ODAHandle* odaHandle = new ODAHandle(startOffset, endOffset);
			if (! collectValues(md, *odaHandle))
			{
				L << "FastODA2Request@" << this << "::scanFile: collectValues returned false" << std::endl;
				return false;
			}
			currentMD.reset(md.clone());
			ASSERT(currentMD->equalsIncludingConstants(md, columnNames_));

			offsets.push_back(startOffset);
			lengths.push_back(blockSize);
			handles.push_back(odaHandle);
		}
		else
		{
			L << "FastODA2Request@" << this << "::scanFile: append <" << startOffset << "," << endOffset << "> to existing handle" << std::endl;

			ODAHandle& lastHandle = *(handles.back());
			lastHandle.end(lastHandle.end() + blockSize);
			lengths.back() += blockSize;
		}
		rowsNumber_ += md.rowsNumber();
	}
	Log::debug() << "FastODA2Request@" << this << "::scanFile => offsets=" << offsets << std::endl;
	Log::debug() << "FastODA2Request@" << this << "::scanFile => lengths=" << lengths << std::endl;
	Log::debug() << "FastODA2Request@" << this << "::scanFile => rowsNumber_=" << rowsNumber_ << std::endl;
	return true;
}

template <typename T>
bool FastODA2Request<T>::collectValues(const MetaData& md, ODAHandle& odaHandle)
{
    using eckit::Offset;
    std::ostream& L (eckit::Log::debug());
    
	std::vector<std::string> currentValues;
	for (size_t i = 0; i < columnNames_.size(); ++i)
	{
		const std::string& columnName (columnNames_[i]);

		Column* column = md.hasColumn(columnName) ? md.columnByName(columnName) : 0;
		std::string v = ! column ? columnNotFound(columnName)
				: ! column->isConstant() ? columnIsNotConstant(*column)
                : column->type() == odc::api::STRING ? StringTool::double_as_string(column->min())
                : column->type() == odc::api::INTEGER ? StringTool::int_as_double2string(column->min())
				: eckit::Translator<double, std::string>()(column->min());
		values_[i].insert(v);
		currentValues.push_back(v);
		double dv = !column ? odc::MDI::realMDI() : column->min();

        L << "FastODA2Request@" << this << "::collectValues: columnName: " << columnName << ": " << v << "(" << dv << ")" << std::endl;

		odaHandle.addValue(columnNames_[i], dv);
		doubleValues_[keywords_[i]].insert(dv);
	}

	if (columnNames_.size())
	{
		if (valuesSeen_.find(currentValues) == valuesSeen_.end())
            valuesSeen_[currentValues] = std::make_pair<Offset,Offset>(odaHandle.start(), odaHandle.end());
		else {
			std::pair<Offset,Offset> p = valuesSeen_[currentValues];
			std::vector<std::string> vs = columnNames_;
			for (size_t i = 0; i < vs.size(); ++i)
				vs[i] += std::string("=") + currentValues[i];
            std::stringstream s;
			s << "Values " << vs << " found in blocks <" << p.first << "," << p.second << ">"
				<< " and <" << odaHandle.start() << "," << odaHandle.end() << ">";
			if (! duplicateCombination(s.str()))
				return false;
		}
	}
	return true;
}

template <typename T>
std::string FastODA2Request<T>::genRequest() const
{
    std::stringstream request;
    std::ostream& L (eckit::Log::debug());

	for (size_t i = 0; i < columnNames_.size(); ++i)
	{
		const std::string& key = keywords_[i];
		std::string k = eckit::StringTools::upper(key);
		std::string valuesList;
        const std::set<std::string>& vs = values_[i];
        for (std::set<std::string>::const_iterator vi = vs.begin(); vi != vs.end(); ++vi)
			valuesList += std::string(vi != vs.begin() ? "/" : "") + patchValue(k, *vi);
		if (i > 0)
			request << ",\n";
		request << key << " = " << valuesList;
	}

    L << "FastODA2Request@" << this << "::genRequest() => " << std::endl << request.str() << std::endl;
	
	return request.str();
}

template <typename T>
std::string FastODA2Request<T>::patchValue(const std::string& k, const std::string& value) const
{
    using eckit::Log;
    using eckit::StringTools;
    std::ostream& L (Log::debug());
    
	std::string v = StringTools::trim(value);
	L << "FastODA2Request@" << this << "::patchValue: v = '" << v  << "', key = " << k << std::endl;
	if (k == "TIME")
		v = StringTool::patchTimeForMars(v);
	else if (k == "CLASS" || k == "TYPE" || k == "STREAM" || k == "OBSGROUP")
	{
		L << "FastODA2Request@" << this << "::genRequest: checking if '" << v << "' is numeric" << std::endl;
		if (StringTool::check(v, isdigit))
		{
			L << "FastODA2Request@" << this << "::genRequest: replacing " << v << " with ";
			v = GribCodes::alphanumeric(StringTools::lower(k), v);
			L << v << std::endl;
		}
		v = StringTools::upper(v);
	}
	return v;
}

template <typename T>
const std::set<std::string>& FastODA2Request<T>::getValues(const std::string& keyword)
{
	for (size_t i = 0; i < keywords_.size(); ++i)
		if (keywords_[i] == keyword)
			return values_[i];
	throw eckit::UserError(std::string("Keyword '") + keyword + "' not found");
	// This is to keep the compiler happy:
	return values_[-1];
}

template <typename T>
std::map<std::string, std::vector<double> > FastODA2Request<T>::getValues()
{
	std::map<std::string, std::vector<double> > r;

	for (std::map<std::string, std::set<double> >::const_iterator it = doubleValues_.begin(); it != doubleValues_.end(); ++it)
	{
		const std::string &k = it->first;
		const std::set<double>& values = it->second;

		std::vector<double>& vs = r[k] = std::vector<double>();

		for (std::set<double>::const_iterator vi = values.begin(); vi != values.end(); ++vi)
			vs.push_back(*vi);
	}
	return r;
}

template <typename T>
std::map<std::string, double> FastODA2Request<T>::getUniqueValues()
{
	std::map<std::string, double> r;
	for (size_t i (0); i < keywords_.size(); ++i)
	{
		std::string kw (keywords_[i]);
		if ( doubleValues_[kw].size() != 1)
		{
            std::stringstream s;
            std::set<double>& values (doubleValues_[kw]);
			s << "Data should contain only one '" << kw << "' value, found: " << values.size() << ". ";
            for (std::set<double>::iterator it (values.begin()); it != values.end(); ++it)
                s << *it << ", ";
			throw eckit::UserError(s.str());
		}
		r[kw] = *doubleValues_[kw].begin();
	}
	return r;
}

} // namespace odc

