/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Simon Smart
/// @date January 2019

#ifndef odc_api_Odb_H
#define odc_api_Odb_H

#include <string>
#include <memory>
#include <vector>

#include "odc/api/ColumnType.h"
#include "odc/api/StridedData.h"

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

class TableImpl;
class DecodeTarget;

class Table {

public: // methods

    Table(std::shared_ptr<TableImpl> t);
    ~Table();

    size_t numRows() const;
    size_t numColumns() const;

    const std::string& columnName(int col) const;
    ColumnType columnType(int col) const;
    size_t columnDecodedSize(int col) const;

    void decode(DecodeTarget& target) const;

private: // members

    std::shared_ptr<TableImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

class DecodeTargetImpl;

class DecodeTarget {

public: // methods

    DecodeTarget(std::vector<StridedData>& columnFacades);
    ~DecodeTarget();

private: // members

    std::shared_ptr<DecodeTargetImpl> impl_;

    friend class Table;
};

//----------------------------------------------------------------------------------------------------------------------

class OdbImpl;

class Odb {

public: // methods

    Odb(const std::string& path);
    ~Odb();

    // Get the vector of Tables. We only read _files_ with this interface,
    // so it doesn't matter that this implies multiple passes.

    const std::vector<Table>& tables();

    int numTables();

private: // members

    std::shared_ptr<OdbImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
