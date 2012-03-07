/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "LSTool.h"

#include "MetaData.h"
#include "Column.h"
#include "SQLSelectFactory.h"

namespace odb {
namespace tool {

ToolFactory<LSTool> lsTool("ls");

LSTool::LSTool (int argc, char *argv[]) : Tool(argc, argv)
{
	registerOptionWithArgument("-o"); // Text Output
	registerOptionWithArgument("-O"); // Reader Output
	registerOptionWithArgument("-s"); // Select
	registerOptionWithArgument("-w"); // Where
}

const char * LSTool::defaultColumns = "andate,date,antime,time,reportype";

const string LSTool::nullString;

unsigned long long LSTool::runSQLSelect(const string &db, const string &selectList, const string &whereClause, ostream &out, const string &odaOutput)
{
	string sql = string("select ") + selectList + " ";
	if (odaOutput != nullString)
		sql += string("into \"") + odaOutput +"\" ";

	sql += "from \"" + db + "\" ";

	if (whereClause != nullString)
		sql += "where " + whereClause;
	sql += ";";

	Log::debug() << "runSQLSelect: " << sql << endl;

	odb::sql::SQLInteractiveSession session(out);
	odb::sql::SQLParser p;
	p.parseString(sql, static_cast<DataHandle*>(0), odb::sql::SQLSelectFactory::instance().config());
	return session.lastExecuteResult();
}

unsigned long long LSTool::runFast(const string &db, const vector<string> &columns, ostream &out)
{
	odb::Reader oda(db);
	odb::Reader::iterator row = oda.begin();
	odb::Reader::iterator end = oda.end();

	vector<size_t> indices;
	vector<odb::ColumnType> types;

	odb::MetaData &md = row->columns();
	for (size_t i = 0; i < columns.size(); ++i)
	{
		size_t index = md.columnIndex(columns[i]);
		indices.push_back(index);
	
		odb::Column &column = *md[index];
		odb::ColumnType type = column.type();
		types.push_back(type);

		out << column.name() << "\t";
	}
	out << endl;

	// Formatting of real values:
	out << fixed;

	unsigned long long n = 0;
	for ( ; row != end; ++row, ++n)
	{
		for (size_t c = 0; c < indices.size(); ++c)
		{
			size_t index = indices[c];
			switch(types[c])
			{
				case odb::INTEGER:
				case odb::BITFIELD:
					//out << row.integer(index);
					out << static_cast<int>((*row)[index]);
					break;
				case odb::REAL:
					out << (*row)[index];
					break;
				case odb::STRING:
					out << "'" << (*row).string(index) << "'";
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
	string selectList = optionArgument("-s", string(defaultColumns));
	vector<string> columns = split(",", selectList);

	auto_ptr<ofstream> foutPtr;
	if (optionIsSet("-o"))
		foutPtr.reset(new ofstream(optionArgument("-o", string("")).c_str()));
	ostream& out = optionIsSet("-o") ? *foutPtr : cout;

	string odaOutput = optionArgument("-O", nullString);

	unsigned long long n = 0;

	// TODO: If -O set and  -w not set then process without SQL

	if (! (optionIsSet("-w") || optionIsSet("-O") || selectList == "*"))
		n = runFast(db, columns, out);
	else
		n = runSQLSelect(db, selectList, optionArgument("-w", nullString), out, odaOutput);

	Log::info() << "Selected " << n << " row(s)." << endl;
}

} // namespace tool 
} // namespace odb 

