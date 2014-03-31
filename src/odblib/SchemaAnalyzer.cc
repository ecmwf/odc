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
#include "odblib/StringTool.h"

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

void SchemaAnalyzer::beginSchema(const string& name)
{
    if (!currentSchema_.empty())
    {
        string message = "Cannot create new schema '" + name
            + "' - current schema '" + currentSchema_ + "' not finalized";
        throw eclib::UserError(message);
    }

    pair<SchemaDefs::iterator, bool> result;
    result = schemas_.insert(make_pair(name, SchemaDef()));

    if (result.second == false)
    {
        string message = "Schema '" + name + "' already defined";
        throw eclib::UserError(message);
    }

    currentSchema_ = name;
}

void SchemaAnalyzer::endSchema()
{
    currentSchema_.clear();
}

void SchemaAnalyzer::addTable(TableDef& table) {
    Log::debug() << "SchemaAnalyzer::addTable:" << table.name() << endl;
    string schemaName = "";

    if (StringTool::isInQuotes(table.name())) table.name(StringTool::unQuote(table.name()));
    else {
        size_t pos = table.name().find(".");
        if (pos != string::npos) {
            schemaName = table.name().substr(0, pos);
            table.name(table.name().substr(pos + 1));
        }
    }

	ColumnDefs& columns (table.columns());
	for (ColumnDefs::iterator it = columns.begin(); it != columns.end(); ++it) {
        ColumnDef& column(*it);
        column.name(column.name() + "@" + table.name());
        columnTypes_[column.name()] = column.type();

        if (isBitfield(column.name()))
            column.bitfieldDef(getBitfieldTypeDefinition(column.name()));
	}

    for (int i = 0, n = table.parents().size(); i < n; i++) {
        TableDefs::const_iterator it(tableDefs_.find(table.parents()[i]));

        if (it == tableDefs_.end())
            throw eclib::UserError(string("Could not find definition of parent table '")
                     + table.parents()[i] + "' inherited by table '" + table.name() + "'");

        const TableDef& parent (it->second);
        if(! parent.parents().empty()) throw UserError("More than 1-level inheritance not supported");

        for (ColumnDefs::const_iterator c (parent.columns().begin()); c != parent.columns().end(); ++c)
            table.columns().push_back(*c);
    }

    if (currentSchema_.empty() && schemaName.empty()) {
        pair<TableDefs::iterator, bool> result (tableDefs_.insert(pair<string, TableDef>(table.name(), table)));
        if (result.second == false)
            throw eclib::UserError(string ("Table '") + table.name() + "' already defined");
    } else {
        if (schemaName.empty())
            schemaName = currentSchema_;

        SchemaDefs::iterator it (schemas_.find(schemaName));
        if (it == schemas_.end())
            throw eclib::UserError(string("Referenced schema '") + schemaName + "' not defined '");

        SchemaDef& schema (it->second);
        TableDefs& tables (schema.tables());

        pair<TableDefs::iterator, bool> result (tables.insert(pair<string, TableDef>(table.name(), table)));
        if (result.second == false) 
            throw eclib::UserError(string ("Table '") + table.name() + "' already defined in '" + schemaName + "' schema");
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
			const string typeName = i->type();
			if (typeName == "@LINK") {
				Log::info() << "SchemaAnalyzer::generateSELECT: Skipping " << i->name() << endl;
				continue;
			}
			selectList += i->name() + ", ";
		}
		selectList += "\n";
	}
	return "\nSELECT\n" + selectList + "\n FROM\n" + from;
}

Definitions SchemaAnalyzer::generateDefinitions()
{
    return Definitions(schemas_, tableDefs_);
}

void SchemaAnalyzer::addBitfieldType(const string name, const FieldNames& fields, const Sizes& sizes, const string typeSignature)
{
	//Log::debug() << "SchemaAnalyzer::addBitfieldType: " << name << "(" << typeSignature << ")" << endl;
	bitfieldTypes_[name] = make_pair(fields, sizes);
}

bool SchemaAnalyzer::isBitfield(const string columnName) const
{
    ASSERT(columnTypes_.find(columnName) != columnTypes_.end());
    if (columnTypes_.find(columnName) == columnTypes_.end())
        return false;
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
		Column &c (*md[i]);
		if (c.type() == BITFIELD) {
            //Log::info() << "colname = " << c.name() << " truename = " << truenames[c.name()] << endl;
			c.bitfieldDef(const_cast<SchemaAnalyzer*>(this)->getBitfieldTypeDefinition(truenames[c.name()]));
        }
	}
}

bool SchemaAnalyzer::tableKnown(const string& name) const
{
    return tableDefs_.find(name) != tableDefs_.end();
}

const TableDef& SchemaAnalyzer::findTable(const string& name) const
{
    for (TableDefs::const_iterator it(tableDefs_.begin()); it != tableDefs_.end(); ++it)
    {
        Log::info() << "SchemaAnalyzer::findTable: " << it->first << endl;
        if (it->first == name)
            return it->second;
    }
    throw eclib::UserError(string("Table '" + name + "' not found"));
}

} // namespace sql
} // namespace odb 

