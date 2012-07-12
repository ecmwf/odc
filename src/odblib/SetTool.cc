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
#include "eclib/Tokenizer.h"
#include "ConstantSetter.h"
#include "SetTool.h"

namespace odb {
namespace tool {

SetTool::SetTool (int argc, char *parameters[]) : Tool(argc, parameters) { }

void SetTool::run()
{
	if (parameters().size() != 4)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	vector<string> columns;
	vector<double> values;

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

void SetTool::parseUpdateList(string s, vector<string>& columns, vector<double>& values)
{
    Tokenizer splitAssignments(",");
    vector<string> assignments;
    splitAssignments(s, assignments);
	
    Tokenizer splitEq("=");

	for (size_t i = 0; i < assignments.size(); ++i)
	{
		vector<string> assignment;
		splitEq(assignments[i], assignment);
		ASSERT(assignment.size() == 2);

		string colName = assignment[0];
		string value = assignment[1];
		
		Log::info() << "SetTool::parseUpdateList: " << colName << "='" << value << "'" << endl;

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
				string byteInHex = value.substr(i * 2, 2);
				char *p = 0;
				unsigned char x;
				reinterpret_cast<unsigned char*>(&v)[i] = x = static_cast<unsigned char>(strtoul(byteInHex.c_str(), &p, 16));
				Log::debug() << "SetTool::parseUpdateList: '" << byteInHex << "' => " << x << endl;
			}
		}

		columns.push_back(colName);
		values.push_back(v);
	}
}

} // namespace tool 
} // namespace odb 

