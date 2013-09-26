/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/SQLAST.h"

using namespace std;

namespace odb {
namespace sql {

ColumnDef::ColumnDef()
{}

ColumnDef::ColumnDef(const string& name, const string& type, const Range& range,
        const string& defaultValue)
  : name_(name),
    type_(type),
    defaultValue_(defaultValue),
    range_(range)
{}

ConstraintDef::ConstraintDef()
  : type_(NONE),
    name_(""),
    columns_(0),
    relatedTable_(""),
    relatedColumns_(0)
{}

ConstraintDef::ConstraintDef(const string& name, const vector<string>& primaryKey)
  : type_(PRIMARY_KEY),
    name_(name),
    columns_(primaryKey),
    relatedTable_(""),
    relatedColumns_(0)
{}

ConstraintDef::ConstraintDef(const string& name, const vector<string>& foreignKey,
        const string& relatedTable, const vector<string>& relatedColumns)
  : type_(FOREIGN_KEY),
    name_(name),
    columns_(foreignKey),
    relatedTable_(relatedTable),
    relatedColumns_(relatedColumns)
{}

TableDef::TableDef()
{}

TableDef::TableDef(const string& name, const ColumnDefs& columns,
        const ConstraintDefs& constraints, const vector<string>& parents)
  : name_(name),
    columns_(columns),
    constraints_(constraints),
    parents_(parents)
{}

SchemaDef::SchemaDef()
{}

SchemaDef::SchemaDef(const TableDefs& tables)
  : tables_(tables)
{}

Definitions::Definitions()
{}

Definitions::Definitions(const SchemaDefs& schemas, const TableDefs& tables)
  : schemas_(schemas),
    tables_(tables)
{}

} // namespace sql
} // namespace odb
