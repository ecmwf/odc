/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// ECMWF Oct 2010

#ifndef TODATable_H
#define TODATable_H

#include "eckit/sql/SQLTable.h"
#include "odb_api/Reader.h"


namespace odb {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class TODATable : public eckit::sql::SQLTable {
public:

    TODATable(eckit::sql::SQLDatabase& owner, const std::string& path, const std::string& name);
    TODATable(eckit::sql::SQLDatabase& owner, eckit::DataHandle& dh);
//    TODATable(eckit::sql::SQLDatabase&, std::istream&, const std::string& delimiter);

    virtual ~TODATable();

    const Reader& oda() const;

private: // methods

    void populateMetaData();
//    void updateMetaData(const std::vector<SQLColumn*>&);

private: // methods (overrides)

    virtual bool hasColumn(const std::string&) const override;
    virtual const eckit::sql::SQLColumn& column(const std::string&) const override;

    virtual eckit::sql::SQLTableIterator* iterator(const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>&,
                                                   std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) const override;

    virtual void print(std::ostream& s) const override;

public:

    Reader oda_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql 
} // namespace odb 

#endif
