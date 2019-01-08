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

#ifndef odc_api_Table_H
#define odc_api_Table_H

#include <memory>
#include <cstddef>

#include "odc/api/ColumnType.h"

namespace odc {
namespace api {

class TableImpl;

//----------------------------------------------------------------------------------------------------------------------

class Table {

public: // methods

    Table(std::shared_ptr<TableImpl> t);
    ~Table();

    size_t numRows() const;
    size_t numColumns() const;

    const std::string& columnName(int col) const;
    ColumnType columnType(int col) const;

private: // members

    std::shared_ptr<TableImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
