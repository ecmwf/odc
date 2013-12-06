/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/LSTool.h"
#include "odblib/Reader.h"

using namespace eckit;

namespace odb {
namespace tool {

LSTool::LSTool (int argc, char *argv[]) : Tool(argc, argv)
{
	registerOptionWithArgument("-o"); // Text Output
}

const std::string LSTool::nullString;

unsigned long long LSTool::printData(const std::string &db, std::ostream &out)
{
	odb::Reader f(db);
	odb::Reader::iterator it = f.begin();
	odb::Reader::iterator end = f.end();

	odb::MetaData md(0);
	// Formatting of real values:
    out << std::fixed;
	unsigned long long n = 0;
	for ( ; it != end; ++it, ++n)
	{
		if (md != it->columns())
		{
			md = it->columns();
			for (size_t i = 0; i < md.size(); ++i)
				out << md[i]->name() << "\t";
			out << std::endl;
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
				case odb::DOUBLE:
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
		out << std::endl;
	}
	return n;
}

void LSTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

    std::string db = parameters(1);

    std::auto_ptr<std::ofstream> foutPtr;
	if (optionIsSet("-o"))
        foutPtr.reset(new std::ofstream(optionArgument("-o", std::string("")).c_str()));
    std::ostream& out = optionIsSet("-o") ? *foutPtr : std::cout;

	unsigned long long n = 0;
	n = printData(db, out);
	Log::info() << "Selected " << n << " row(s)." << std::endl;
}

} // namespace tool 
} // namespace odb 

