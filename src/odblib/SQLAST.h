/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file SQLAST.h
/// @author Piotr Kuchta, ECMWF April 2009

#ifndef SQLAST_H
#define SQLAST_H

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace odb {
namespace sql {

typedef std::pair<long,long> Range;

class ColumnDef
{
public:
	ColumnDef();
	ColumnDef(const std::string& name, const std::string& type, const Range& range,
		const std::string& defaultValue);

	const std::string& name() const { return name_; }
	const std::string& type() const { return type_; }
	const std::string& defaultValue() const { return defaultValue_; }
	const Range& range() const { return range_; }
private:
        std::string name_;
        std::string type_;
        std::string defaultValue_;
	Range range_;
};

typedef std::vector<ColumnDef> ColumnDefs; 

class ConstraintDef
{
    enum Type { NONE, PRIMARY_KEY, FOREIGN_KEY };
public:
	ConstraintDef();

        ConstraintDef(const std::string& name,
            const std::vector<std::string>& primaryKey); 

        ConstraintDef(const std::string& name,
            const std::vector<std::string>& foreignKey, const std::string& relatedTable,
            const std::vector<std::string>& relatedColumn); 

        bool isPrimaryKey() const { return type_ == PRIMARY_KEY; }
        bool isForeignKey() const { return type_ == FOREIGN_KEY; }

        const std::string& name() const { return name_; }
        const std::vector<std::string>& columns() const { return columns_; }
        const std::string& relatedTable() const { return relatedTable_; }
        const std::vector<std::string>& relatedColumns() const { return relatedColumns_; }
private:
        Type type_;
        std::string name_;
        std::vector<std::string> columns_;
        std::string relatedTable_;
        std::vector<std::string> relatedColumns_;
};

typedef std::vector<ConstraintDef> ConstraintDefs; 

class TableDef
{
public:
	TableDef();
	TableDef(const std::string& name, const ColumnDefs& columns,
            const ConstraintDefs& constraints, const std::vector<std::string>& parents);

	const std::string& name() const { return name_; }
	ColumnDefs& columns() { return columns_; }
	const ColumnDefs& columns() const { return columns_; }
        const ConstraintDefs& constraints() const { return constraints_; }
        const std::vector<std::string>& parents() const { return parents_; }
private:
        std::string name_;
	ColumnDefs columns_;
        ConstraintDefs constraints_;
	std::vector<std::string> parents_;
};

typedef std::map<std::string, TableDef> TableDefs;

class SchemaDef
{
public:
	SchemaDef(const TableDefs& tables);
	const TableDefs& tables() const { return tables_; }
private:
	TableDefs tables_;
};

} // namespace sql
} // namespace odb

#endif

