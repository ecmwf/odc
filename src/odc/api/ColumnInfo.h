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

/** Describes a column and the data encoded in it */
struct ColumnInfo {

    /** Describes a bit or bit group in a bitfield */
    struct Bit {
        /** Bit group name */
        std::string name;
        /** Bit group size in bits */
        int size;
        /** Bit group offset in bits */
        int offset;
    };

    /** Column name */
    std::string name;
    /** Column data type */
    ColumnType type;
    /** Size of a single decoded value in bytes */
    size_t decodedSize;
    /** List of bit and bit groups associated with a bitfield column */
    std::vector<Bit> bitfield;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace api
}  // namespace odc

#endif  // odc_api_StridedData_H
