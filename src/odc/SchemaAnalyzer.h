/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file SchemaAnalyzer.h
/// @author Piotr Kuchta, ECMWF April 2009

#ifndef odc_SchemaAnalyzer_H
#define odc_SchemaAnalyzer_H


namespace odc {

class MetaData;

namespace sql {

class SchemaAnalyzer {

public:

    SchemaAnalyzer();
    ~SchemaAnalyzer();

    void beginSchema(const std::string& name);
    void endSchema();
    void addTable(TableDef& table);

    void addBitfieldType(const std::string& name, const FieldNames& fields, const Sizes& sizes, const std::string& typeSignature);
    bool isBitfield(const std::string& columnName) const; 
    const eckit::sql::BitfieldDef& getBitfieldTypeDefinition(const std::string& columnName);
    void updateBitfieldsDefs(MetaData &, std::map<std::string, std::string> &) const;
    bool tableKnown(const std::string& name) const;
    const eckit::sql::TableDef& findTable(const std::string& name) const;
    void skipTable(std::string tableName);
    std::string generateSELECT() const;
    eckit::sql::Definitions generateDefinitions();
    std::string findColumnType(const std::string&);

private:
    std::string currentSchema_;
    eckit::sql::SchemaDefs schemas_;
    eckit::sql::TableDefs tableDefs_;
    eckit::sql::BitfieldDefs bitfieldTypes_;
    std::set<std::string> tablesToSkip_;
    std::map<std::string,std::string> columnTypes_;

};

} // namespace sql
} // namespace odc

#endif

