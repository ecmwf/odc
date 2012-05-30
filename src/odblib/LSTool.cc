/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/oda.h"

#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/LSTool.h"
#include "odblib/MetaData.h"
#include "odblib/Column.h"
#include "odblib/SQLSelectFactory.h"

namespace odb {
namespace tool {

ToolFactory<LSTool> lsTool("ls");

LSTool::LSTool (int argc, char *argv[]) : Tool(argc, argv)
{
	registerOptionWithArgument("-o"); // Text Output
}

const string LSTool::nullString;

unsigned long long LSTool::runFast(const string &db, ostream &out)
{
	odb::Reader f(db);
	odb::Reader::iterator it = f.begin();
	odb::Reader::iterator end = f.end();

	odb::MetaData md(0);
	// Formatting of real values:
	out << fixed;
	unsigned long long n = 0;
	for ( ; it != end; ++it, ++n)
	{
		if (md != it->columns())
		{
			md = it->columns();
			for (size_t i = 0; i < md.size(); ++i)
				out << md[i]->name() << "\t";
			out << endl;
		}
		for (size_t i = 0; i < md.size(); ++i)
		{
			switch(md[i]->type())
			{
				case odb::INTEGER:
				case odb::BITFIELD:
					out << static_cast<int>((*it)[i]);
					break;
				case odb::REAL:
					out << (*it)[i];
					break;
				case odb::STRING:
					out << "'" << (*it).string(i) << "'";
					break;
				case odb::IGNORE:
				default:
					ASSERT("Unknown type" && false);
					break;
			}
			out << "\t";
		}
		out << endl;
	}
	return n;
}

void LSTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	string db = parameters(1);

	auto_ptr<ofstream> foutPtr;
	if (optionIsSet("-o"))
		foutPtr.reset(new ofstream(optionArgument("-o", string("")).c_str()));
	ostream& out = optionIsSet("-o") ? *foutPtr : cout;

	unsigned long long n = 0;
	n = runFast(db, out);
	Log::info() << "Selected " << n << " row(s)." << endl;
}

} // namespace tool 
} // namespace odb 

