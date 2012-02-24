///
/// \file TemplateParameters.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "oda.h"
#include "TemplateParameters.h"

#define SRC __FILE__, __LINE__

namespace odb {

TemplateParameters::TemplateParameters()
: vector<TemplateParameter*>()
{}

TemplateParameters::~TemplateParameters() { release(); }

void TemplateParameters::release()
{
	for (size_t i = 0; i < size(); ++i)
		delete at(i);
	clear();
}

MetaData TemplateParameters::nullMD(0);

TemplateParameters& TemplateParameters::parse(const string& fileNameTemplate, TemplateParameters& params, const MetaData& columns)
{
	const string &t = fileNameTemplate;

	int start = 0;	
	for (;;)
	{
		size_t l = t.find("{", start);
		if (l == string::npos)
			break;
		start = l;
		size_t r = t.find("}", start);
		if (r == string::npos)
			throw UserError(string("TemplateParameters::parse: missing '}' while parsing '") + fileNameTemplate + "'");
		start = r;

		string name = t.substr(l + 1, r - l - 1);
		size_t index = &columns != &nullMD ? columns.columnIndex(name) : -1;

		params.push_back(new TemplateParameter(l, r, index, name));	

		Log::debug() << "TemplateParameters::parse: new param: " << l << ", " << r << ", " << index << ", " << name << endl;
	}
	return params;
}

} // namespace odb
