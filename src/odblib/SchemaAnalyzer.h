/// @file SchemaAnalyzer.h
/// @author Piotr Kuchta, ECMWF April 2009

#ifndef SchemaAnalyzer_H
#define SchemaAnalyzer_H

#include "SQLBitfield.h"

#include <set>
#include <map>

namespace odb {

class MetaData;

namespace sql {

class SchemaAnalyzer {

public:
	SchemaAnalyzer();
	~SchemaAnalyzer();

	void addTable(sql::TableDef);
	void addBitfieldType(const string name, const FieldNames& fields, const Sizes& sizes, const string typeSignature);
	bool isBitfield(const string columnName) const; 
	const BitfieldDef& getBitfieldTypeDefinition(const string columnName); 
	void updateBitfieldsDefs(MetaData &) const;

	void skipTable(string tableName);
	string generateSELECT() const;

private:
	sql::TableDefs tableDefs_;
	BitfieldDefs bitfieldTypes_;

	set<string> tablesToSkip_;
	map<string,string> columnTypes_;
};

} // namespace sql
} // namespace odb

#endif

