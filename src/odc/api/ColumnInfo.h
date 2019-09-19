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
/// @date March 2019

#ifndef odc_api_ColumnInfo_H
#define odc_api_ColumnInfo_H

#include <string>
#include <vector>

#include "odc/api/ColumnType.h"

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

struct ColumnInfo {

    struct Bit {
        std::string name;
        int size;
        int offset;
    };

    std::string name;
    ColumnType type;
    size_t decodedSize;
    std::vector<Bit> bitfield;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_StridedData_H
