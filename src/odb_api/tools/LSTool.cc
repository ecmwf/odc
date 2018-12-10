/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/Reader.h"
#include "odb_api/tools/LSTool.h"

using namespace eckit;

namespace odc {
namespace tool {

LSTool::LSTool (int argc, char *argv[]) : Tool(argc, argv)
{
	registerOptionWithArgument("-o"); // Text Output
}

const std::string LSTool::nullString;

unsigned long long LSTool::printData(const std::string &db, std::ostream &out)
{
	odc::Reader f(db);
	odc::Reader::iterator it = f.begin();
	odc::Reader::iterator end = f.end();

	odc::MetaData md(0);
	// Formatting of real values:
    out << std::fixed;
	unsigned long long n = 0;
	for ( ; it != end; ++it, ++n)
	{
		if (md != it->columns())
		{
			md = it->columns();
            const char* spacer = "";
            for (size_t i = 0; i < md.size(); ++i) {
                out << spacer << md[i]->name();
                spacer = "\t";
            }
			out << std::endl;
		}
        const char* spacer = "";
        for (size_t i = 0; i < md.size(); ++i)
		{
            out << spacer;
			switch(md[i]->type())
			{
				case odc::INTEGER:
				case odc::BITFIELD:
					out << static_cast<int>((*it)[i]);
					break;
				case odc::REAL:
				case odc::DOUBLE:
					out << (*it)[i];
					break;
				case odc::STRING:
					out << "'" << (*it).string(i) << "'";
					break;
				case odc::IGNORE:
				default:
					ASSERT("Unknown type" && false);
					break;
			}
            spacer = "\t";
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

    std::unique_ptr<std::ofstream> foutPtr;
	if (optionIsSet("-o"))
        foutPtr.reset(new std::ofstream(optionArgument("-o", std::string("")).c_str()));
    std::ostream& out = optionIsSet("-o") ? *foutPtr : std::cout;

	unsigned long long n = 0;
	n = printData(db, out);
	Log::info() << "Selected " << n << " row(s)." << std::endl;
}

} // namespace tool 
} // namespace odc 

