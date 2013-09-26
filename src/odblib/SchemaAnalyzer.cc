/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/odb_api.h"
#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"

using namespace eclib;

namespace odb {
namespace sql {

SchemaAnalyzer::SchemaAnalyzer()
{
//desc, hdr, sat, reo3, satob, rtovs, rtovs_slev, rtovs_mlev, rtovs_pred, atovs, atovs_pred, scatt, ssmi, ssmi_slev, ssmi_mlev, body, errstat, update, rtovs_body, ssmi_body, scatt_body
	skipTable("poolmask");
	skipTable("bufr");
	skipTable("ddrs");
	skipTable("index");
	skipTable("rtovs");
	skipTable("rtovs_slev");
	skipTable("rtovs_mlev");
	skipTable("rtovs_pred");
	skipTable("rtovs_body");
	skipTable("ssmi");
	skipTable("ssmi_body");
	skipTable("ssmi_slev");
	skipTable("ssmi_pred");
	skipTable("ssmi_mlev");
	skipTable("atovs");
	skipTable("atovs_pred");
	skipTable("scatt");
	skipTable("scatt_body");
	skipTable("reo3");
	skipTable("sat");
	skipTable("satob");
	skipTable("update");
	skipTable("");
	skipTable("");
} 

SchemaAnalyzer::~SchemaAnalyzer() {}

void SchemaAnalyzer::addSchema(const string& name)
{
	pair<set<string>::iterator, bool> result;
        result = schemaSet_.insert(name);

        if (!result.second)
        {
            string message = "Schema '" + name + "' already exits";
            throw eclib::UserError(message);
        }
}
void SchemaAnalyzer::addTable(const TableDef& table)
{
	pair<TableDefs::iterator, bool> result;
        result = tableDefs_.insert(pair<string, TableDef>(table.name(), table));

        if (result.second == false)
        {
            string message = "Table '" + table.name() + "' already exits";
            throw eclib::UserError(message);
        }

	const ColumnDefs& columns = table.columns();
	for (ColumnDefs::const_iterator i = columns.begin(); i != columns.end(); ++i)
	{
		const string columnName = i->name();
		const string fullName = columnName + "@" + table.name();
		const string typeName = i->type();

		columnTypes_[fullName] = typeName;
		//Log::debug() << "SchemaAnalyzer::addTable: columnTypes_[" << fullName << "] = " << typeName << endl;
	}
}

void SchemaAnalyzer::skipTable(string tableName)
{
	tablesToSkip_.insert(tableName);
}

string SchemaAnalyzer::generateSELECT() const
{
	string from = "";
	string selectList = "";
	if (tableDefs_.size() == 0)
		return "";

	for (TableDefs::const_iterator t = tableDefs_.begin(); t != tableDefs_.end(); ++t)
	{
		TableDef tableDef = t->second;
		string tableName = tableDef.name();

		if (tablesToSkip_.find(tableName) != tablesToSkip_.end())
			continue;

		from += tableName + ", ";
		ColumnDefs columnDefs = tableDef.columns();
		
		for (ColumnDefs::const_iterator i = columnDefs.begin(); i != columnDefs.end(); i++)
		{
			const string columnName = i->name();
			const string fullName = columnName + "@" + tableName;
			const string typeName = i->type();
			if (typeName == "@LINK") {
				Log::info() << "SchemaAnalyzer::generateSELECT: Skipping " << fullName << endl;
				continue;
			}
			selectList += fullName + ", ";
		}
		selectList += "\n";
	}
	return "\nSELECT\n" + selectList + "\n FROM\n" + from;
}

Definitions SchemaAnalyzer::generateDefinitions()
{
    for (TableDefs::iterator t = tableDefs_.begin(); t != tableDefs_.end(); ++t)
    {
        TableDef& table = t->second;
        ColumnDefs& columns = table.columns();
        string tableName = table.name();

        for (ColumnDefs::iterator c = columns.begin(); c != columns.end(); c++)
        {
            // Remove schema qualifier from the table name before
            // we add it as a suffix of collumn.

            string suffix = tableName;
            unsigned pos = suffix.find(".");
            if (pos != string::npos)
                suffix = suffix.substr(pos+1);
            
            ColumnDef& col = *c;
            col = ColumnDef(col.name() + "@" + suffix, col.type(), col.range(),
                    col.defaultValue());
        }
    }

    // NOTE: This algorithm only supports 1-level inheritance.

    SchemaDefs schemas;
    for (TableDefs::iterator t = tableDefs_.begin();
            t != tableDefs_.end(); ++t)
    {
        TableDef& table = t->second;

        for (int i = 0, n = table.parents().size(); i < n; i++)
        {
            const string& parentName = table.parents()[i];
            const TableDef& parent = tableDefs_.at(parentName);

            ASSERT(parent.parents().empty() && "2-level inheritance not supported");

            for (ColumnDefs::const_iterator c = parent.columns().begin();
                    c != parent.columns().end(); ++c)
                table.columns().push_back(*c);
        }

        string tableName = table.name();
        unsigned pos = tableName.find(".");
        if (pos != string::npos)
        {
            string schemaName = tableName.substr(0, pos);
            tableName = tableName.substr(pos+1);
            table.name(tableName);
            schemas[schemaName].tables().insert(make_pair(tableName, table));
        }
    }

    return Definitions(schemas, tableDefs_);
}

void SchemaAnalyzer::addBitfieldType(const string name, const FieldNames& fields, const Sizes& sizes, const string typeSignature)
{
	//Log::debug() << "SchemaAnalyzer::addBitfieldType: " << name << "(" << typeSignature << ")" << endl;
	bitfieldTypes_[name] = make_pair(fields, sizes);
}

bool SchemaAnalyzer::isBitfield(const string columnName) const
{
	ASSERT(columnTypes_.find(columnName) != columnTypes_.end());
	string columnType = columnTypes_.find(columnName)->second;
	return bitfieldTypes_.find(columnType) != bitfieldTypes_.end();
}

const BitfieldDef& SchemaAnalyzer::getBitfieldTypeDefinition(const string columnName) 
{
	ASSERT(isBitfield(columnName));
	string columnType = columnTypes_.find(columnName)->second;
	return bitfieldTypes_[columnType];
}

void SchemaAnalyzer::updateBitfieldsDefs(MetaData &md, map<string,string> & truenames) const
{
	for (size_t i = 0; i < md.size(); i++)
	{
		Column &c = *md[i];
		if (c.type() == BITFIELD) {
            //Log::info() << "colname = " << c.name() << " truename = " << truenames[c.name()] << endl;
			c.bitfieldDef(const_cast<SchemaAnalyzer*>(this)->getBitfieldTypeDefinition(truenames[c.name()]));
        }
	}
}

} // namespace sql
} // namespace odb 

