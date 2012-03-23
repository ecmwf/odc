/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file SchemaAnalyzer.h
/// @author Piotr Kuchta, ECMWF April 2009

#ifndef SchemaAnalyzer_H
#define SchemaAnalyzer_H

#include <set>
#include <map>

#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"

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

