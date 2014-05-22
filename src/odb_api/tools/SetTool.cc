/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <strings.h>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/parser/Tokenizer.h"

#include "odb_api/ConstantSetter.h"
#include "odb_api/Reader.h"
#include "odb_api/Writer.h"

#include "SetTool.h"

using namespace eckit;

namespace odb {
namespace tool {

SetTool::SetTool (int argc, char *parameters[]) : Tool(argc, parameters) { }

void SetTool::run()
{
	if (parameters().size() != 4)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

	std::vector<std::string> columns;
	std::vector<double> values;

	PathName inFile = parameters(2);
	PathName outFile = parameters(3);

	odb::Reader in(inFile);
	odb::Writer<> out(outFile);

	odb::Writer<>::iterator writer(out.begin());

	odb::Reader::iterator sourceIt = in.begin();
	const odb::Reader::iterator sourceEnd = in.end();

	parseUpdateList(parameters(1), columns, values);

	typedef odb::ConstantSetter<odb::Reader::iterator> Setter;
	Setter setter(sourceIt, sourceEnd, columns, values);
	Setter::iterator begin = setter.begin();
	const Setter::iterator end = setter.end();
	writer->pass1(begin, end);
}

void SetTool::parseUpdateList(std::string s, std::vector<std::string>& columns, std::vector<double>& values)
{
    Tokenizer splitAssignments(",");
    std::vector<std::string> assignments;
    splitAssignments(s, assignments);
	
    Tokenizer splitEq("=");

	for (size_t i = 0; i < assignments.size(); ++i)
	{
		std::vector<std::string> assignment;
		splitEq(assignments[i], assignment);
		ASSERT(assignment.size() == 2);

		std::string colName = assignment[0];
		std::string value = assignment[1];
		
		Log::info() << "SetTool::parseUpdateList: " << colName << "='" << value << "'" << std::endl;

		double v = 0;

		if (value.find("0x") != 0)
			v = translate(value); 
		else
		{
			value = value.substr(2);
			ASSERT("Format of the hexadecimal value is not correct" && (value.size() % 2) == 0);
			ASSERT("Hexadecimal literal is too long" && (value.size() / 2) <= sizeof(double));
		
			bzero(&v, sizeof(double));
			for (size_t i = 0; i < value.size() / 2; ++i)
			{
				std::string byteInHex = value.substr(i * 2, 2);
				char *p = 0;
				unsigned char x;
				reinterpret_cast<unsigned char*>(&v)[i] = x = static_cast<unsigned char>(strtoul(byteInHex.c_str(), &p, 16));
				Log::debug() << "SetTool::parseUpdateList: '" << byteInHex << "' => " << x << std::endl;
			}
		}

		columns.push_back(colName);
		values.push_back(v);
	}
}

} // namespace tool 
} // namespace odb 

