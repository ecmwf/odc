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

/** Describes a data column */
struct ColumnInfo {

    /** Describes a bitfield */
    struct Bit {
        /** Bitfield name */
        std::string name;
        /** Bitfield size in bits */
        int size;
        /** Bitfield offset in bits */
        int offset;
    };

    /** Column name */
    std::string name;
    /** Column type */
    ColumnType type;
    /** Decoded size of column data in bytes */
    size_t decodedSize;
    /** List of column bitfields */
    std::vector<Bit> bitfield;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_StridedData_H
