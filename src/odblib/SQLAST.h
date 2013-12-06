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

#include <eckit/eckit.h>

//#include <map>
//#include <string>
//#include <utility>
//#include <vector>

#include "odblib/Types.h"

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
        void name(const std::string& name) { name_ = name; }
    const std::string& type() const { return type_; }
	const Range& range() const { return range_; }
    const std::string& defaultValue() const { return defaultValue_; }
        bool hasDefaultValue() const { return hasDefaultValue_; }
        const BitfieldDef& bitfieldDef() const { return bitfieldDef_; }
        void bitfieldDef(const BitfieldDef& b) { bitfieldDef_ = b; }
private:
        std::string name_;
        std::string type_;
	Range range_;
        std::string defaultValue_;
        bool hasDefaultValue_;
        BitfieldDef bitfieldDef_;
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
    void name(const std::string& name) { name_ = name; }
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
	SchemaDef();
	SchemaDef(const TableDefs& tables);
	TableDefs& tables() { return tables_; }
	const TableDefs& tables() const { return tables_; }
private:
	TableDefs tables_;
};

typedef std::map<std::string, SchemaDef> SchemaDefs;

class Definitions
{
public:
    Definitions();
    Definitions(const SchemaDefs& schemas, const TableDefs& tables);
    const SchemaDefs& schemas() const { return schemas_; }
    const TableDefs& tables() const { return tables_; }
private:
    SchemaDefs schemas_;
    TableDefs tables_;
};

} // namespace sql
} // namespace odb

#endif
