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

#ifndef odb_api_sql_TODATableIterator_H
#define odb_api_sql_TODATableIterator_H

#include "eckit/sql/SQLTable.h"
#include "odb_api/Reader.h"


namespace odb {
namespace sql {

class TODATable;

//----------------------------------------------------------------------------------------------------------------------

class TODATableIterator : public eckit::sql::SQLTableIterator {

public: // methods

    TODATableIterator(const TODATable& parent,
                      const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
                      std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback);
	virtual ~TODATableIterator();

private: // methods (override>

    virtual void rewind() override;
    virtual bool next() override;

    virtual std::vector<size_t> columnOffsets() const override;
    virtual const double* data() const override;

private: // methods

    void updateMetaData();

private: // members

    const TODATable& parent_;
    Reader::iterator it_;
    Reader::iterator end_;

    const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns_;
    std::vector<size_t> columnOffsets_;

    std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback_;

	bool firstRow_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odb

#endif
