#include "oda.h"
#include "SQLAST.h"
#include "SchemaAnalyzer.h"

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

void SchemaAnalyzer::addTable(TableDef tableDef)
{
	tableDefs_.push_back(tableDef);

	string tableName = tableDef.first;

	ColumnDefs columnDefs = tableDef.second;
	for (ColumnDefs::const_iterator i = columnDefs.begin(); i != columnDefs.end(); i++)
	{
		const string columnName = i->name();
		const string fullName = columnName + "@" + tableName;
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
		TableDef tableDef = *t;
		string tableName = tableDef.first;

		if (tablesToSkip_.find(tableName) != tablesToSkip_.end())
			continue;

		from += tableName + ", ";
		ColumnDefs columnDefs = tableDef.second;
		
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

void SchemaAnalyzer::updateBitfieldsDefs(MetaData &md) const
{
	for (size_t i = 0; i < md.size(); i++)
	{
		Column &c = *md[i];
		if (c.type() == BITFIELD)
			c.bitfieldDef(const_cast<SchemaAnalyzer*>(this)->getBitfieldTypeDefinition(c.name()));
	}
}

} // namespace sql
} // namespace odb 

