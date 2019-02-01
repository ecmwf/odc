/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file TemplateParameters.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "odc/TemplateParameters.h"

using namespace eckit;
using namespace odc::core;

namespace odc {

TemplateParameters::TemplateParameters()
: std::vector<TemplateParameter*>()
{}

TemplateParameters::~TemplateParameters() { release(); }

void TemplateParameters::release()
{
	for (size_t i = 0; i < size(); ++i)
		delete at(i);
	clear();
}

MetaData TemplateParameters::nullMD(0);

TemplateParameters::TemplateParameters(const std::string& fileNameTemplate, const MetaData& columns)
{
    parse(fileNameTemplate, *this, columns);
}

TemplateParameters& TemplateParameters::parse(const std::string& fileNameTemplate, TemplateParameters& params, const MetaData& columns)
{
	const std::string &t = fileNameTemplate;

	int start = 0;	
	for (;;)
	{
		size_t l = t.find("{", start);
		if (l == std::string::npos)
			break;
		start = l;
		size_t r = t.find("}", start);
		if (r == std::string::npos)
			throw eckit::UserError(std::string("TemplateParameters::parse: missing '}' while parsing '") + fileNameTemplate + "'");
		start = r;

		std::string name = t.substr(l + 1, r - l - 1);
		size_t index = &columns != &nullMD ? columns.columnIndex(name) : -1;

		params.push_back(new TemplateParameter(l, r, index, name));	

		Log::debug() << "TemplateParameters::parse: new param: " << l << ", " << r << ", " << index << ", " << name << std::endl;
	}
	return params;
}

} // namespace odc
